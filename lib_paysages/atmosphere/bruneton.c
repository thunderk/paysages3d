#include "public.h"

/*
 * Atmospheric scattering, based on E. Bruneton and F.Neyret work.
 * http://evasion.inrialpes.fr/~Eric.Bruneton/
 */

#if 1

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../system.h"
#include "../tools/cache.h"
#include "../tools/texture.h"

/*********************** Constants ***********************/

#define TRANSMITTANCE_NON_LINEAR
#define INSCATTER_NON_LINEAR
#define FIX

static const double Rg = 6360.0;
static const double Rt = 6420.0;
static const double RL = 6421.0;
static const double exposure = 0.4;
static const double ISun = 100.0;
static const double AVERAGE_GROUND_REFLECTANCE = 0.1;

#define RES_MU 128
#define RES_MU_S 32
#define RES_R 32
#define RES_NU 8
#define SKY_W 64
#define SKY_H 16
#define TRANSMITTANCE_W 256
#define TRANSMITTANCE_H 64
#define TRANSMITTANCE_INTEGRAL_SAMPLES 500
#define INSCATTER_INTEGRAL_SAMPLES 50
#define IRRADIANCE_INTEGRAL_SAMPLES 32
#define INSCATTER_SPHERICAL_INTEGRAL_SAMPLES 16

Texture2D* _transmittanceTexture = NULL;
Texture2D* _deltaETexture = NULL;
Texture2D* _irradianceTexture = NULL;
Texture3D* _inscatterTexture = NULL;
Texture3D* _deltaSMTexture = NULL;
Texture3D* _deltaSRTexture = NULL;
Texture3D* _deltaJTexture = NULL;

// Rayleigh
static const double HR = 8.0;
static const Color betaR = {5.8e-3, 1.35e-2, 3.31e-2, 1.0};

// Mie
// DEFAULT
static const double HM = 1.2;
static const Vector3 betaMSca = {4e-3, 4e-3, 4e-3};
static const Vector3 betaMEx = {4e-3 / 0.9, 4e-3 / 0.9, 4e-3 / 0.9};
static const double mieG = 0.8;
// CLEAR SKY
/*static const float HM = 1.2;
static const vec3 betaMSca = vec3(20e-3);
static const vec3 betaMEx = betaMSca / 0.9;
static const float mieG = 0.76;*/
// PARTLY CLOUDY
/*static const float HM = 3.0;
static const vec3 betaMSca = vec3(3e-3);
static const vec3 betaMEx = betaMSca / 0.9;
static const float mieG = 0.65;*/

/*********************** Layer variables ***********************/

static double _r;
static Color _dhdH;
static int _layer;

/*********************** Shader helpers ***********************/

#define step(_a_,_b_) ((_a_) < (_b_) ? 0 : 1)
#define sign(_a_) ((_a_) < 0.0 ? -1.0 : ((_a_) > 0.0 ? 1.0 : 0.0))
#define mix(_x_,_y_,_a_) ((_x_) * (1.0 - (_a_)) + (_y_) * (_a_))
static inline double min(double a, double b)
{
    return a < b ? a : b;
}
static inline double max(double a, double b)
{
    return a > b ? a : b;
}
static inline Color vec4mix(Color v1, Color v2, double a)
{
    v1.r = mix(v1.r, v2.r, a);
    v1.g = mix(v1.g, v2.g, a);
    v1.b = mix(v1.b, v2.b, a);
    v1.a = mix(v1.a, v2.a, a);
    return v1;
}
static inline double clamp(double x, double minVal, double maxVal)
{
    if (x < minVal)
    {
        x = minVal;
    }
    return (x > maxVal) ? maxVal : x;
}
static inline double smoothstep(double edge0, double edge1, double x)
{
    double t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}
static inline void _fixVec4Min(Color* vec, double minVal)
{
    if (vec->r < minVal) { vec->r = minVal; }
    if (vec->g < minVal) { vec->g = minVal; }
    if (vec->b < minVal) { vec->b = minVal; }
    if (vec->a < minVal) { vec->a = minVal; }
}
static inline Color vec4max(Color vec, double minVal)
{
    if (vec.r < minVal) { vec.r = minVal; }
    if (vec.g < minVal) { vec.g = minVal; }
    if (vec.b < minVal) { vec.b = minVal; }
    if (vec.a < minVal) { vec.a = minVal; }
    return vec;
}

static inline Vector3 vec3(double x, double y, double z)
{
    Vector3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

static inline Color vec4(double r, double g, double b, double a)
{
    Color result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

/*********************** Texture manipulation ***********************/

static inline Color _texture3D(Texture3D* tex, Vector3 p)
{
    return texture3DGetLinear(tex, p.x, p.y, p.z);
}

static Color _texture4D(Texture3D* tex3d, double r, double mu, double muS, double nu)
{
    double H = sqrt(Rt * Rt - Rg * Rg);
    double rho = sqrt(r * r - Rg * Rg);
#ifdef INSCATTER_NON_LINEAR
    double rmu = r * mu;
    double delta = rmu * rmu - r * r + Rg * Rg;
    Color cst = (rmu < 0.0 && delta > 0.0) ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / (double)(RES_MU)) : vec4(-1.0, H * H, H, 0.5 + 0.5 / (double)(RES_MU));
    double uR = 0.5 / (double)(RES_R) + rho / H * (1.0 - 1.0 / (double)(RES_R));
    double uMu = cst.a + (rmu * cst.r + sqrt(delta + cst.g)) / (rho + cst.b) * (0.5 - 1.0 / (double)(RES_MU));
    // paper formula
    //float uMuS = 0.5 / (double)(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / (double)(RES_MU_S));
    // better formula
    double uMuS = 0.5 / (double)(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / (double)(RES_MU_S));
#else
    float uR = 0.5 / (double)(RES_R) + rho / H * (1.0 - 1.0 / (double)(RES_R));
    float uMu = 0.5 / (double)(RES_MU) + (mu + 1.0) / 2.0 * (1.0 - 1.0 / (double)(RES_MU));
    float uMuS = 0.5 / (double)(RES_MU_S) + max(muS + 0.2, 0.0) / 1.2 * (1.0 - 1.0 / (double)(RES_MU_S));
#endif
    double lerp = (nu + 1.0) / 2.0 * ((double)(RES_NU) - 1.0);
    double uNu = floor(lerp);
    lerp = lerp - uNu;
    return vec4mix(_texture3D(tex3d, vec3((uNu + uMuS + 1.0) / (double)(RES_NU), uMu, uR)), _texture3D(tex3d, vec3((uNu + uMuS) / (double)(RES_NU), uMu, uR)), lerp);
}

/*********************** Physics functions ***********************/

/* Rayleigh phase function */
static double _phaseFunctionR(double mu)
{
    return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
}

/* Mie phase function */
static double _phaseFunctionM(double mu)
{
    return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG * mieG) * pow(1.0 + (mieG * mieG) - 2.0 * mieG * mu, -3.0 / 2.0) * (1.0 + mu * mu) / (2.0 + mieG * mieG);
}

/* approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision") */
static Color _getMie(Color rayMie)
{
    Color result;

    result.r = rayMie.r * rayMie.a / max(rayMie.r, 1e-4) * (betaR.r / betaR.r);
    result.g = rayMie.g * rayMie.a / max(rayMie.r, 1e-4) * (betaR.r / betaR.g);
    result.b = rayMie.b * rayMie.a / max(rayMie.r, 1e-4) * (betaR.r / betaR.b);
    result.a = 1.0;

    return result;
}

/* optical depth for ray (r,mu) of length d, using analytic formula
   (mu=cos(view zenith angle)), intersections with ground ignored
   H=height scale of exponential density function */
static double _opticalDepth(double H, double r, double mu, double d)
{
    double a = sqrt((0.5 / H) * r);
    double ax = a * (mu);
    double ay = a * (mu + d / r);
    double axs = sign(ax);
    double ays = sign(ay);
    double axq = ax * ax;
    double ayq = ay * ay;
    double x = ays > axs ? exp(axq) : 0.0;
    double yx = axs / (2.3193 * fabs(ax) + sqrt(1.52 * axq + 4.0));
    double yy = ays / (2.3193 * fabs(ay) + sqrt(1.52 * ayq + 4.0)) *  exp(-d / H * (d / (2.0 * r) + mu));
    return sqrt((6.2831 * H) * r) * exp((Rg - r) / H) * (x + yx - yy);
}

static inline void _getTransmittanceUV(double r, double mu, double* u, double* v)
{
    double dr = (r - Rg) / (Rt - Rg);
#ifdef TRANSMITTANCE_NON_LINEAR
    if (dr >= 0.0)
    {
        *v = sqrt(dr);
    }
    else
    {
        *v = 0.0;
    }
    *u = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;
#else
    *v = (r - Rg) / (Rt - Rg);
    *u = (mu + 0.15) / (1.0 + 0.15);
#endif
}

/* transmittance(=transparency) of atmosphere for infinite ray (r,mu)
   (mu=cos(view zenith angle)), intersections with ground ignored */
static Color _transmittance(double r, double mu)
{
    double u, v;
    _getTransmittanceUV(r, mu, &u, &v);
    return texture2DGetLinear(_transmittanceTexture, u, v);
}

/* transmittance(=transparency) of atmosphere between x and x0
 * assume segment x,x0 not intersecting ground
 * d = distance between x and x0, mu=cos(zenith angle of [x,x0) ray at x) */
Color _transmittance3(double r, double mu, double d)
{
    Color result, t1, t2;
    double r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);
    double mu1 = (r * mu + d) / r1;
    if (mu > 0.0)
    {
        t1 = _transmittance(r, mu);
        t2 = _transmittance(r1, mu1);
    }
    else
    {
        t1 = _transmittance(r1, -mu1);
        t2 = _transmittance(r, -mu);
    }
    result.r = min(t1.r / t2.r, 1.0);
    result.g = min(t1.g / t2.g, 1.0);
    result.b = min(t1.b / t2.b, 1.0);
    result.a = 1.0;
    return result;
}

static void _getIrradianceRMuS(double x, double y, double* r, double* muS)
{
    *r = Rg + y * (Rt - Rg);
    *muS = -0.2 + x * (1.0 + 0.2);
}

/* nearest intersection of ray r,mu with ground or top atmosphere boundary
 * mu=cos(ray zenith angle at ray origin) */
static double _limit(double r, double mu)
{
    double dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
    double delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
    if (delta2 >= 0.0)
    {
        double din = -r * mu - sqrt(delta2);
        if (din >= 0.0) {
            dout = min(dout, din);
        }
    }
    return dout;
}

static double _opticalDepthTransmittance(double H, double r, double mu)
{
    double result = 0.0;
    double dx = _limit(r, mu) / (double)TRANSMITTANCE_INTEGRAL_SAMPLES;
    double xi = 0.0;
    double yi = exp(-(r - Rg) / H);
    int i;
    for (i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i) {
        double xj = (double)i * dx;
        double yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - Rg) / H);
        result += (yi + yj) / 2.0 * dx;
        xi = xj;
        yi = yj;
    }
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 1e9 : result;
}

static void _getTransmittanceRMu(double x, double y, double* r, double* muS)
{
#ifdef TRANSMITTANCE_NON_LINEAR
    *r = Rg + (y * y) * (Rt - Rg);
    *muS = -0.15 + tan(1.5 * x) / tan(1.5) * (1.0 + 0.15);
#else
    *r = Rg + y * (Rt - Rg);
    *muS = -0.15 + x * (1.0 + 0.15);
#endif
}

/* transmittance(=transparency) of atmosphere for ray (r,mu) of length d
   (mu=cos(view zenith angle)), intersections with ground ignored
   uses analytic formula instead of transmittance texture */
static Vector3 _analyticTransmittance(double r, double mu, double d)
{
    Vector3 result;
    double opt = _opticalDepth(HR, r, mu, d);

    result.x = exp(-betaR.r * opt) - betaMEx.x * opt;
    result.y = exp(-betaR.g * opt) - betaMEx.y * opt;
    result.z = exp(-betaR.b * opt) - betaMEx.z * opt;

    return result;
}

static inline Color _applyInscatter(Color inscatter, Color attmod, Color samp)
{
    inscatter.r = inscatter.r - attmod.r * samp.r;
    inscatter.g = inscatter.g - attmod.g * samp.g;
    inscatter.b = inscatter.b - attmod.b * samp.b;
    inscatter.a = inscatter.a - attmod.a * samp.a;
    return vec4max(inscatter, 0.0);
}

/* transmittance(=transparency) of atmosphere for infinite ray (r,mu)
   (mu=cos(view zenith angle)), or zero if ray intersects ground */
static Color _transmittanceWithShadow(double r, double mu)
{
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? COLOR_BLACK : _transmittance(r, mu);
}

static Color _hdr(Color c1, Color c2, Color c3)
{
    Color L = {c1.r + c2.r + c3.r, c1.g + c2.g + c3.g, c1.b + c2.b + c3.b, 1.0};

    L.r *= exposure;
    L.g *= exposure;
    L.b *= exposure;
    L.a *= exposure;

    L.r = L.r < 1.413 ? pow(L.r * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.r);
    L.g = L.g < 1.413 ? pow(L.g * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.g);
    L.b = L.b < 1.413 ? pow(L.b * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.b);

    return L;
}

static void _getMuMuSNu(double x, double y, double r, Color dhdH, double* mu, double* muS, double* nu)
{
#ifdef INSCATTER_NON_LINEAR
    double d;
    if (y < (double)(RES_MU) / 2.0)
    {
        d = 1.0 - y / ((double)(RES_MU) / 2.0);
        d = min(max(dhdH.b, d * dhdH.a), dhdH.a * 0.999);
        *mu = (Rg * Rg - r * r - d * d) / (2.0 * r * d);
        *mu = min(*mu, -sqrt(1.0 - (Rg / r) * (Rg / r)) - 0.001);
    }
    else
    {
        double d = (y - (double)(RES_MU) / 2.0) / ((double)(RES_MU) / 2.0);
        d = min(max(dhdH.r, d * dhdH.g), dhdH.g * 0.999);
        *mu = (Rt * Rt - r * r - d * d) / (2.0 * r * d);
    }
    *muS = fmod(x, (double)(RES_MU_S)) / ((double)(RES_MU_S));
    /* paper formula :
     *   muS = -(0.6 + log(1.0 - muS * (1.0 -  exp(-3.6)))) / 3.0; */
    /* better formula */
    *muS = tan((2.0 * (*muS) - 1.0 + 0.26) * 1.1) / tan(1.26 * 1.1);
    *nu = -1.0 + floor(x / (double)(RES_MU_S)) / ((double)(RES_NU)) * 2.0;
#else
    mu = -1.0 + 2.0 * y / (float(RES_MU) - 1.0);
    muS = mod(x, float(RES_MU_S)) / (float(RES_MU_S) - 1.0);
    muS = -0.2 + muS * 1.2;
    nu = -1.0 + floor(x / float(RES_MU_S)) / (float(RES_NU) - 1.0) * 2.0;
#endif
}

static void _getIrradianceUV(double r, double muS, double* uMuS, double* uR)
{
    *uR = (r - Rg) / (Rt - Rg);
    *uMuS = (muS + 0.2) / (1.0 + 0.2);
}

static Color _irradiance(Texture2D* sampler, double r, double muS)
{
    double u, v;
    _getIrradianceUV(r, muS, &u, &v);
    return texture2DGetLinear(sampler, u, v);
}

/*********************** Texture precomputing ***********************/

static void _precomputeTransmittanceTexture()
{
    int x, y;

    for (x = 0; x < TRANSMITTANCE_W; x++)
    {
        for (y = 0; y < TRANSMITTANCE_H; y++)
        {
            double r, muS;
            _getTransmittanceRMu((double)x / TRANSMITTANCE_W, (double)y / TRANSMITTANCE_H, &r, &muS);
            double depth1 = _opticalDepthTransmittance(HR, r, muS);
            double depth2 = _opticalDepthTransmittance(HM, r, muS);
            Color trans;
            trans.r = exp(-(betaR.r * depth1 + betaMEx.x * depth2));
            trans.g = exp(-(betaR.g * depth1 + betaMEx.y * depth2));
            trans.b = exp(-(betaR.b * depth1 + betaMEx.z * depth2));
            trans.a = 1.0;
            texture2DSetPixel(_transmittanceTexture, x, y, trans); /* Eq (5) */
        }
    }
}

static void _precomputeIrrDeltaETexture()
{
    int x, y;

    /* Irradiance program */
    for (x = 0; x < SKY_W; x++)
    {
        for (y = 0; y < SKY_H; y++)
        {
            double r, muS, u, v;
            Color trans, irr;
            _getIrradianceRMuS((double)x / SKY_W, (double)y / SKY_H, &r, &muS);
            trans = _transmittance(r, muS);

            _getTransmittanceUV(r, muS, &u, &v);

            irr.r = trans.r * max(muS, 0.0);
            irr.g = trans.g * max(muS, 0.0);
            irr.b = trans.b * max(muS, 0.0);
            irr.a = 1.0;

            texture2DSetPixel(_deltaETexture, x, y, irr);
        }
    }
}

static void _setLayer(int layer)
{
    double r = layer / (RES_R - 1.0);
    r = r * r;
    r = sqrt(Rg * Rg + r * (Rt * Rt - Rg * Rg)) + (layer == 0 ? 0.01 : (layer == RES_R - 1 ? -0.001 : 0.0));
    double dmin = Rt - r;
    double dmax = sqrt(r * r - Rg * Rg) + sqrt(Rt * Rt - Rg * Rg);
    double dminp = r - Rg;
    double dmaxp = sqrt(r * r - Rg * Rg);

    _r = r;
    _dhdH.r = dmin;
    _dhdH.g = dmax;
    _dhdH.b = dminp;
    _dhdH.a = dmaxp;
    _layer = layer;
}

/*********************** inscatter1.glsl ***********************/

static void _integrand1(double r, double mu, double muS, double nu, double t, Color* ray, Color* mie)
{
    double ri = sqrt(r * r + t * t + 2.0 * r * mu * t);
    double muSi = (nu * t + muS * r) / ri;
    ri = max(Rg, ri);
    if (muSi >= -sqrt(1.0 - Rg * Rg / (ri * ri)))
    {
        Color t1, t2;
        t1 = _transmittance3(r, mu, t);
        t2 = _transmittance(ri, muSi);
        double fR = exp(-(ri - Rg) / HR);
        double fM = exp(-(ri - Rg) / HM);
        ray->r = fR * t1.r * t2.r;
        ray->g = fR * t1.g * t2.g;
        ray->b = fR * t1.b * t2.b;
        mie->r = fM * t1.r * t2.r;
        mie->g = fM * t1.g * t2.g;
        mie->b = fM * t1.b * t2.b;
    }
    else
    {
        ray->r = ray->g = ray->b = 0.0;
        mie->r = mie->g = mie->b = 0.0;
    }
}

static void _inscatter1(double r, double mu, double muS, double nu, Color* ray, Color* mie)
{
    ray->r = ray->g = ray->b = 0.0;
    mie->r = mie->g = mie->b = 0.0;
    double dx = _limit(r, mu) / (double)(INSCATTER_INTEGRAL_SAMPLES);
    double xi = 0.0;
    Color rayi;
    Color miei;
    _integrand1(r, mu, muS, nu, 0.0, &rayi, &miei);
    int i;
    for (i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i)
    {
        double xj = (double)(i) * dx;
        Color rayj;
        Color miej;
        _integrand1(r, mu, muS, nu, xj, &rayj, &miej);
        ray->r += (rayi.r + rayj.r) / 2.0 * dx;
        ray->g += (rayi.g + rayj.g) / 2.0 * dx;
        ray->b += (rayi.b + rayj.b) / 2.0 * dx;
        mie->r += (miei.r + miej.r) / 2.0 * dx;
        mie->g += (miei.g + miej.g) / 2.0 * dx;
        mie->b += (miei.b + miej.b) / 2.0 * dx;
        xi = xj;
        rayi = rayj;
        miei = miej;
    }
    ray->r *= betaR.r;
    ray->g *= betaR.g;
    ray->b *= betaR.b;
    mie->r *= betaMSca.x;
    mie->g *= betaMSca.y;
    mie->b *= betaMSca.z;
}

static void _inscatter1Prog(Texture3D* tex_rayleigh, Texture3D* tex_mie)
{
    int x, y;
    for (x = 0; x < RES_MU_S * RES_NU; x++)
    {
        /*double dx = (double)x / (double)(RES_MU_S * RES_NU);*/
        for (y = 0; y < RES_MU; y++)
        {
            /*double dy = (double)y / (double)(RES_MU);*/

            Color ray = COLOR_BLACK;
            Color mie = COLOR_BLACK;
            double mu, muS, nu;
            _getMuMuSNu((double)x, (double)y, _r, _dhdH, &mu, &muS, &nu);
            _inscatter1(_r, mu, muS, nu, &ray, &mie);
            /* store separately Rayleigh and Mie contributions, WITHOUT the phase function factor
             * (cf "Angular precision") */
            texture3DSetPixel(tex_rayleigh, x, y, _layer, ray);
            texture3DSetPixel(tex_mie, x, y, _layer, mie);
        }
    }
}

/*********************** inscatterS.glsl ***********************/

static Color _inscatterS(double r, double mu, double muS, double nu, int first)
{
    Color raymie = COLOR_BLACK;

    double dphi = M_PI / (double)(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);
    double dtheta = M_PI / (double)(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);

    r = clamp(r, Rg, Rt);
    mu = clamp(mu, -1.0, 1.0);
    muS = clamp(muS, -1.0, 1.0);
    double var = sqrt(1.0 - mu * mu) * sqrt(1.0 - muS * muS);
    nu = clamp(nu, muS * mu - var, muS * mu + var);

    double cthetamin = -sqrt(1.0 - (Rg / r) * (Rg / r));

    Vector3 v = vec3(sqrt(1.0 - mu * mu), 0.0, mu);
    double sx = v.x == 0.0 ? 0.0 : (nu - muS * mu) / v.x;
    Vector3 s = vec3(sx, sqrt(max(0.0, 1.0 - sx * sx - muS * muS)), muS);

    /* integral over 4.PI around x with two nested loops over w directions (theta,phi) -- Eq (7) */
    int itheta;
    for (itheta = 0; itheta < INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++itheta)
    {
        double theta = ((double)(itheta) + 0.5) * dtheta;
        double ctheta = cos(theta);

        double greflectance = 0.0;
        double dground = 0.0;
        Color gtransp = {0.0, 0.0, 0.0, 0.0};
        if (ctheta < cthetamin)
        {
            /* if ground visible in direction w
             * compute transparency gtransp between x and ground */
            greflectance = AVERAGE_GROUND_REFLECTANCE / M_PI;
            dground = -r * ctheta - sqrt(r * r * (ctheta * ctheta - 1.0) + Rg * Rg);
            gtransp = _transmittance3(Rg, -(r * ctheta + dground) / Rg, dground);
        }

        int iphi;
        for (iphi = 0; iphi < 2 * INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++iphi)
        {
            double phi = ((double)(iphi) + 0.5) * dphi;
            double dw = dtheta * dphi * sin(theta);
            Vector3 w = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), ctheta);

            double nu1 = v3Dot(s, w);
            double nu2 = v3Dot(v, w);
            double pr2 = _phaseFunctionR(nu2);
            double pm2 = _phaseFunctionM(nu2);

            /* compute irradiance received at ground in direction w (if ground visible) =deltaE */
            Vector3 gnormal;
            gnormal.x = dground * w.x / Rg;
            gnormal.y = dground * w.y / Rg;
            gnormal.z = r + dground * w.z / Rg;
            Color girradiance = _irradiance(_deltaETexture, Rg, v3Dot(gnormal, s));

            Color raymie1; /* light arriving at x from direction w */

            /* first term = light reflected from the ground and attenuated before reaching x, =T.alpha/PI.deltaE */
            raymie1.r = greflectance * girradiance.r * gtransp.r;
            raymie1.g = greflectance * girradiance.g * gtransp.g;
            raymie1.b = greflectance * girradiance.b * gtransp.b;

            /* second term = inscattered light, =deltaS */
            if (first)
            {
                /* first iteration is special because Rayleigh and Mie were stored separately,
                 * without the phase functions factors; they must be reintroduced here */
                double pr1 = _phaseFunctionR(nu1);
                double pm1 = _phaseFunctionM(nu1);
                Color ray1 = _texture4D(_deltaSRTexture, r, w.z, muS, nu1);
                Color mie1 = _texture4D(_deltaSMTexture, r, w.z, muS, nu1);
                raymie.r += ray1.r * pr1 + mie1.r + pm1;
                raymie.g += ray1.g * pr1 + mie1.g + pm1;
                raymie.b += ray1.b * pr1 + mie1.b + pm1;
            }
            else
            {
                Color col = _texture4D(_deltaSRTexture, r, w.z, muS, nu1);
                raymie1.r += col.r;
                raymie1.g += col.g;
                raymie1.b += col.b;
            }

            /* light coming from direction w and scattered in direction v
               = light arriving at x from direction w (raymie1) * SUM(scattering coefficient * phaseFunction)
               see Eq (7) */
            raymie.r += raymie1.r * (betaR.r * exp(-(r - Rg) / HR) * pr2 + betaMSca.x * exp(-(r - Rg) / HM) * pm2) * dw;
            raymie.g += raymie1.g * (betaR.g * exp(-(r - Rg) / HR) * pr2 + betaMSca.y * exp(-(r - Rg) / HM) * pm2) * dw;
            raymie.b += raymie1.b * (betaR.b * exp(-(r - Rg) / HR) * pr2 + betaMSca.z * exp(-(r - Rg) / HM) * pm2) * dw;
        }
    }

    /* output raymie = J[T.alpha/PI.deltaE + deltaS] (line 7 in algorithm 4.1) */
    return raymie;
}

static void _jProg(Texture3D* result, int first)
{
    int x, y;
    for (x = 0; x < RES_MU_S * RES_NU; x++)
    {
        for (y = 0; y < RES_MU; y++)
        {
            Color raymie;
            double mu, muS, nu;
            _getMuMuSNu((double)x, (double)y, _r, _dhdH, &mu, &muS, &nu);
            raymie = _inscatterS(_r, mu, muS, nu, first);
            texture3DSetPixel(result, x, y, _layer, raymie);
        }
    }
}

/*********************** irradianceN.glsl ***********************/

void _irradianceNProg(Texture2D* tex, int first)
{
    int x, y;
    double dphi = M_PI / (double)(IRRADIANCE_INTEGRAL_SAMPLES);
    double dtheta = M_PI / (double)(IRRADIANCE_INTEGRAL_SAMPLES);
    for (x = 0; x < SKY_W; x++)
    {
        for (y = 0; y < SKY_H; y++)
        {
            double r, muS;
            int iphi;
            _getIrradianceRMuS((double)x / SKY_W, (double)y / SKY_H, &r, &muS);
            Vector3 s = vec3(max(sqrt(1.0 - muS * muS), 0.0), 0.0, muS);

            Color result = COLOR_BLACK;
            /* integral over 2.PI around x with two nested loops over w directions (theta,phi) -- Eq (15) */
            for (iphi = 0; iphi < 2 * IRRADIANCE_INTEGRAL_SAMPLES; ++iphi)
            {
                double phi = ((double)(iphi) + 0.5) * dphi;
                int itheta;
                for (itheta = 0; itheta < IRRADIANCE_INTEGRAL_SAMPLES / 2; ++itheta)
                {
                    double theta = ((double)(itheta) + 0.5) * dtheta;
                    double dw = dtheta * dphi * sin(theta);
                    Vector3 w = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
                    double nu = v3Dot(s, w);
                    if (first)
                    {
                        /* first iteration is special because Rayleigh and Mie were stored separately,
                           without the phase functions factors; they must be reintroduced here */
                        double pr1 = _phaseFunctionR(nu);
                        double pm1 = _phaseFunctionM(nu);
                        Color ray1 = _texture4D(_deltaSRTexture, r, w.z, muS, nu);
                        Color mie1 = _texture4D(_deltaSMTexture, r, w.z, muS, nu);
                        result.r += (ray1.r * pr1 + mie1.r * pm1) * w.z * dw;
                        result.g += (ray1.g * pr1 + mie1.g * pm1) * w.z * dw;
                        result.b += (ray1.b * pr1 + mie1.b * pm1) * w.z * dw;
                    }
                    else
                    {
                        Color col = _texture4D(_deltaSRTexture, r, w.z, muS, nu);
                        result.r += col.r * w.z * dw;
                        result.g += col.g * w.z * dw;
                        result.b += col.b * w.z * dw;
                    }
                }
            }

            texture2DSetPixel(tex, x, y, result);
        }
    }
}

/*********************** Final getters ***********************/

/* inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0) */
static Color _getInscatterColor(Vector3* _x, double* _t, Vector3 v, Vector3 s, double* _r, double* _mu, Vector3* attenuation)
{
    Color result;
    double r = v3Norm(*_x);
    double mu = v3Dot(*_x, v) / r;
    double d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);
    if (d > 0.0)
    {
        /* if x in space and ray intersects atmosphere
           move x to nearest intersection of ray with top atmosphere boundary */
        _x->x += d * v.x;
        _x->y += d * v.y;
        _x->z += d * v.z;
        *_t -= d;
        mu = (r * mu + d) / Rt;
        r = Rt;
    }
    double t = *_t;
    Vector3 x = *_x;
    if (r <= Rt)
    {
        /* if ray intersects atmosphere */
        double nu = v3Dot(v, s);
        double muS = v3Dot(x, s) / r;
        double phaseR = _phaseFunctionR(nu);
        double phaseM = _phaseFunctionM(nu);
        Color inscatter = vec4max(_texture4D(_inscatterTexture, r, mu, muS, nu), 0.0);
        if (t > 0.0)
        {
            Vector3 x0 = v3Add(x, v3Scale(v, t));
            double r0 = v3Norm(x0);
            double rMu0 = v3Dot(x0, v);
            double mu0 = rMu0 / r0;
            double muS0 = v3Dot(x0, s) / r0;
#ifdef FIX
            /* avoids imprecision problems in transmittance computations based on textures */
            *attenuation = _analyticTransmittance(r, mu, t);
#else
            *attenuation = _transmittance(r, mu, v, x0);
#endif
            if (r0 > Rg + 0.01)
            {
                /* computes S[L]-T(x,x0)S[L]|x0 */
                Color attmod = {attenuation->x, attenuation->y, attenuation->z, attenuation->x};
                Color samp = _texture4D(_inscatterTexture, r0, mu0, muS0, nu);
                inscatter = _applyInscatter(inscatter, attmod, samp);
#ifdef FIX
                /* avoids imprecision problems near horizon by interpolating between two points above and below horizon */
                const double EPS = 0.004;
                double muHoriz = -sqrt(1.0 - (Rg / r) * (Rg / r));
                if (fabs(mu - muHoriz) < EPS)
                {
                    double a = ((mu - muHoriz) + EPS) / (2.0 * EPS);

                    mu = muHoriz - EPS;
                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
                    mu0 = (r * mu + t) / r0;
                    Color inScatter0 = _texture4D(_inscatterTexture, r, mu, muS, nu);
                    Color inScatter1 = _texture4D(_inscatterTexture, r0, mu0, muS0, nu);
                    Color inScatterA = _applyInscatter(inScatter0, attmod, inScatter1);

                    mu = muHoriz + EPS;
                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
                    mu0 = (r * mu + t) / r0;
                    inScatter0 = _texture4D(_inscatterTexture, r, mu, muS, nu);
                    inScatter1 = _texture4D(_inscatterTexture, r0, mu0, muS0, nu);
                    Color inScatterB = _applyInscatter(inScatter0, attmod, inScatter1);

                    inscatter = vec4mix(inScatterA, inScatterB, a);
                }
#endif
            }
        }
#ifdef FIX
        /* avoids imprecision problems in Mie scattering when sun is below horizon */
        inscatter.a *= smoothstep(0.00, 0.02, muS);
#endif
        Color mie = _getMie(inscatter);
        result.r = inscatter.r * phaseR + mie.r * phaseM;
        result.g = inscatter.g * phaseR + mie.g * phaseM;
        result.b = inscatter.b * phaseR + mie.b * phaseM;
        result.a = inscatter.a * phaseR + mie.a * phaseM;
        _fixVec4Min(&result, 0.0);
    }
    else
    {
        /* x in space and ray looking in space */
        result = COLOR_BLACK;
    }

    *_r = r;
    *_mu = mu;
    result.r *= ISun;
    result.g *= ISun;
    result.b *= ISun;
    result.a = 1.0;
    return result;
}

/*ground radiance at end of ray x+tv, when sun in direction s
 *attenuated bewteen ground and viewer (=R[L0]+R[L*]) */
/*static Color _groundColor(Vector3 x, double t, Vector3 v, Vector3 s, double r, double mu, Color attenuation)
{
    Color result;
    if (t > 0.0)
    { // if ray hits ground surface

        // ground reflectance at end of ray, x0
        Vector3 x0 = v3Add(x, v3Scale(v, t));
        float r0 = v3Norm(x0);
        Vector3 n = v3Scale(x0, 1.0 / r0);
        vec2 coords = vec2(atan(n.y, n.x), acos(n.z)) * vec2(0.5, 1.0) / M_PI + vec2(0.5, 0.0);
        Color reflectance;
        if (r0 > Rg + 0.01)
        {
            reflectance = vec4(0.4, 0.4, 0.4, 0.0);
        }
        else
        {
            reflectance = texture2D(reflectanceSampler, coords) * vec4(0.2, 0.2, 0.2, 1.0);
        }

        // direct sun light (radiance) reaching x0
        float muS = v3Dot(n, s);
        Color sunLight = _transmittanceWithShadow(r0, muS);

        // precomputed sky light (irradiance) (=E[L*]) at x0
        Color groundSkyLight = irradiance(irradianceSampler, r0, muS);

        // light reflected at x0 (=(R[L0]+R[L*])/T(x,x0))
        Color groundColor = reflectance.rgb * (max(muS, 0.0) * sunLight + groundSkyLight) * ISun / M_PI;

        // water specular color due to sunLight
        if (reflectance.w > 0.0)
        {
            vec3 h = normalize(s - v);
            float fresnel = 0.02 + 0.98 * pow(1.0 - dot(-v, h), 5.0);
            float waterBrdf = fresnel * pow(max(dot(h, n), 0.0), 150.0);
            groundColor += reflectance.w * max(waterBrdf, 0.0) * sunLight * ISun;
        }

        result = attenuation * groundColor; //=R[L0]+R[L*]
    }
    else
    { // ray looking at the sky
        return COLOR_BLACK;
    }
    return result;
}*/

/* direct sun light for ray x+tv, when sun in direction s (=L0) */
static Color _sunColor(Vector3 x, double t, Vector3 v, Vector3 s, double r, double mu)
{
    if (t > 0.0)
    {
        return COLOR_BLACK;
    }
    else
    {
        Color transmittance = r <= Rt ? _transmittanceWithShadow(r, mu) : COLOR_WHITE; /* T(x,xo) */
        double isun = step(cos(M_PI / 180.0), v3Dot(v, s)) * ISun; /* Lsun */
        transmittance.r *= isun;
        transmittance.g *= isun;
        transmittance.b *= isun;
        transmittance.a *= isun;
        return transmittance; /* Eq (9) */
    }
}

/*********************** Cache methods ***********************/

static int _tryLoadCache2D(Texture2D* tex, const char* tag, int order)
{
    CacheFile* cache;
    int xsize, ysize;

    texture2DGetSize(tex, &xsize, &ysize);
    cache = cacheFileCreateAccessor("atmo-br", "png", tag, xsize, ysize, 0, order);
    if (cacheFileIsReadable(cache))
    {
        texture2DLoadFromFile(tex, cacheFileGetPath(cache));
        cacheFileDeleteAccessor(cache);
        return 1;
    }
    else
    {
        cacheFileDeleteAccessor(cache);
        return 0;
    }
}

static void _saveCache2D(Texture2D* tex, const char* tag, int order)
{
    CacheFile* cache;
    int xsize, ysize;

    texture2DGetSize(tex, &xsize, &ysize);
    cache = cacheFileCreateAccessor("atmo-br", "png", tag, xsize, ysize, 0, order);
    if (cacheFileIsWritable(cache))
    {
        texture2DSaveToFile(tex, cacheFileGetPath(cache));
    }
    cacheFileDeleteAccessor(cache);
}

static int _tryLoadCache3D(Texture3D* tex, const char* tag, int order)
{
    CacheFile* cache;
    int xsize, ysize, zsize;

    texture3DGetSize(tex, &xsize, &ysize, &zsize);
    cache = cacheFileCreateAccessor("atmo-br", "png", tag, xsize, ysize, zsize, order);
    if (cacheFileIsReadable(cache))
    {
        texture3DLoadFromFile(tex, cacheFileGetPath(cache));
        cacheFileDeleteAccessor(cache);
        return 1;
    }
    else
    {
        cacheFileDeleteAccessor(cache);
        return 0;
    }
}

static void _saveCache3D(Texture3D* tex, const char* tag, int order)
{
    CacheFile* cache;
    int xsize, ysize, zsize;

    texture3DGetSize(tex, &xsize, &ysize, &zsize);
    cache = cacheFileCreateAccessor("atmo-br", "png", tag, xsize, ysize, zsize, order);
    if (cacheFileIsWritable(cache))
    {
        texture3DSaveToFile(tex, cacheFileGetPath(cache));
    }
    cacheFileDeleteAccessor(cache);
}

/*********************** Public methods ***********************/

void brunetonInit()
{
    int layer, x, y, z, order;

    if (_transmittanceTexture == NULL) /* TEMP */
    {
        /* TODO Deletes */

        /* computes transmittance texture T (line 1 in algorithm 4.1) */
        _transmittanceTexture = texture2DCreate(TRANSMITTANCE_W, TRANSMITTANCE_H);
        if (!_tryLoadCache2D(_transmittanceTexture, "transmittance", 0))
        {
            _precomputeTransmittanceTexture();
            _saveCache2D(_transmittanceTexture, "transmittance", 0);
        }

        /* computes irradiance texture deltaE (line 2 in algorithm 4.1) */
        _deltaETexture = texture2DCreate(SKY_W, SKY_H);
        if (!_tryLoadCache2D(_deltaETexture, "irradianceDeltaE", 0))
        {
            _precomputeIrrDeltaETexture();
            _saveCache2D(_deltaETexture, "irradianceDeltaE", 0);
        }

        /* computes single scattering texture deltaS (line 3 in algorithm 4.1)
         * Rayleigh and Mie separated in deltaSR + deltaSM */
        _deltaSRTexture = texture3DCreate(RES_MU_S * RES_NU, RES_MU, RES_R);
        _deltaSMTexture = texture3DCreate(RES_MU_S * RES_NU, RES_MU, RES_R);
        if (!_tryLoadCache3D(_deltaSRTexture, "deltaSR", 0) || !_tryLoadCache3D(_deltaSMTexture, "deltaSM", 0))
        {
            for (layer = 0; layer < RES_R; ++layer)
            {
                printf("deltaS %d\n", layer);
                _setLayer(layer);
                _inscatter1Prog(_deltaSRTexture, _deltaSMTexture);
            }
            _saveCache3D(_deltaSRTexture, "deltaSR", 0);
            _saveCache3D(_deltaSMTexture, "deltaSM", 0);
        }

        /* copies deltaE into irradiance texture E (line 4 in algorithm 4.1) */
        /* ??? all black texture ??? */
        /*_irradianceTexture = texture3DCreate(SKY_W, SKY_H);
        _copyIrradianceProg(0.0, _irrDeltaETexture);*/

        /* copies deltaS into inscatter texture S (line 5 in algorithm 4.1) */
        _inscatterTexture = texture3DCreate(RES_MU_S * RES_NU, RES_MU, RES_R);
        if (!_tryLoadCache3D(_inscatterTexture, "inscatter", 0))
        {
            for (x = 0; x < RES_MU_S * RES_NU; x++)
            {
                for (y = 0; y < RES_MU; y++)
                {
                    for (z = 0; z < RES_R; z++)
                    {
                        Color result = texture3DGetPixel(_deltaSRTexture, x, y, z);
                        Color mie = texture3DGetPixel(_deltaSMTexture, x, y, z);
                        result.a = mie.r;
                        texture3DSetPixel(_inscatterTexture, x, y, z, result);
                    }
                }
            }
            _saveCache3D(_inscatterTexture, "inscatter", 0);
        }

        /* loop for each scattering order (line 6 in algorithm 4.1) */
        for (order = 2; order <= 4; ++order)
        {
            /* computes deltaJ (line 7 in algorithm 4.1) */
            _deltaJTexture = texture3DCreate(RES_MU_S * RES_NU, RES_MU, RES_R);
            if (!_tryLoadCache3D(_deltaJTexture, "deltaJ", order))
            {
                for (layer = 0; layer < RES_R; ++layer)
                {
                    _setLayer(layer);
                    _jProg(_deltaJTexture, order == 2);
                }
                _saveCache3D(_deltaJTexture, "deltaJ", order);
            }

            /* computes deltaE (line 8 in algorithm 4.1) */
            _deltaETexture = texture2DCreate(SKY_W, SKY_H);
            if (!_tryLoadCache2D(_deltaETexture, "deltaE", order))
            {
                _irradianceNProg(_deltaETexture, order == 2);
                _saveCache2D(_deltaETexture, "deltaE", order);
            }

            /* computes deltaS (line 9 in algorithm 4.1) */
            /*glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, deltaSRTexture, 0);
            glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);
            glUseProgram(inscatterNProg);
            glUniform1f(glGetUniformLocation(inscatterNProg, "first"), order == 2 ? 1.0 : 0.0);
            glUniform1i(glGetUniformLocation(inscatterNProg, "transmittanceSampler"), transmittanceUnit);
            glUniform1i(glGetUniformLocation(inscatterNProg, "deltaJSampler"), deltaJUnit);
            for (int layer = 0; layer < RES_R; ++layer) {
                setLayer(inscatterNProg, layer);
                drawQuad();
            }*/

            /*glEnable(GL_BLEND);
            glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);*/

            /* adds deltaE into irradiance texture E (line 10 in algorithm 4.1) */
            /*glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, irradianceTexture, 0);
            glViewport(0, 0, SKY_W, SKY_H);
            glUseProgram(copyIrradianceProg);
            glUniform1f(glGetUniformLocation(copyIrradianceProg, "k"), 1.0);
            glUniform1i(glGetUniformLocation(copyIrradianceProg, "deltaESampler"), deltaEUnit);
            drawQuad();*/

            /* adds deltaS into inscatter texture S (line 11 in algorithm 4.1) */
            /*glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, inscatterTexture, 0);
            glViewport(0, 0, RES_MU_S * RES_NU, RES_MU);
            glUseProgram(copyInscatterNProg);
            glUniform1i(glGetUniformLocation(copyInscatterNProg, "deltaSSampler"), deltaSRUnit);
            for (int layer = 0; layer < RES_R; ++layer) {
                setLayer(copyInscatterNProg, layer);
                drawQuad();
            }
            glDisable(GL_BLEND);*/

            /* DEBUG */
            break;
        }
    }
}

Color brunetonGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position)
{
    Vector3 x = {0.0, Rg + eye.y, 0.0};
    Vector3 v = v3Normalize(direction);
    Vector3 s = v3Normalize(v3Sub(sun_position, eye));

    double r = v3Norm(x);
    double mu = v3Dot(x, v) / r;
    double t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rg * Rg);

    Vector3 g = {0.0, 0.0, Rg + 10.0};
    g = v3Sub(x, g);
    double a = v.x * v.x + v.y * v.y - v.z * v.z;
    double b = 2.0 * (g.x * v.x + g.y * v.y - g.z * v.z);
    double c = g.x * g.x + g.y * g.y - g.z * g.z;
    double d = -(b + sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
    int cone = d > 0.0 && fabs(x.z + d * v.z - Rg) <= 10.0;

    if (t > 0.0)
    {
        if (cone && d < t)
        {
            t = d;
        }
    }
    else if (cone)
    {
        t = d;
    }

    Vector3 attenuation;
    Color inscatterColor = _getInscatterColor(&x, &t, v, s, &r, &mu, &attenuation); //S[L]-T(x,xs)S[l]|xs
    /*Color groundColor = _groundColor(x, t, v, s, r, mu, attenuation); //R[L0]+R[L*]*/
    Color groundColor = COLOR_BLACK;
    Color sunColor = _sunColor(x, t, v, s, r, mu); //L0
    return _hdr(sunColor, groundColor, inscatterColor); // Eq (16)
}
#else
Color brunetonGetSkyColor(AtmosphereDefinition* definition, Vector3 eye, Vector3 direction, Vector3 sun_position)
{
    return COLOR_BLACK;
}
#endif
