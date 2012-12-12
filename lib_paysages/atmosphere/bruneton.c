#include "public.h"

/*
 * Atmospheric scattering, based on E. Bruneton and F.Neyret work.
 * http://evasion.inrialpes.fr/~Eric.Bruneton/
 */

#if 1

#include <math.h>
#include <stdlib.h>
#include "../system.h"

#define TRANSMITTANCE_NON_LINEAR
#define INSCATTER_NON_LINEAR
#define FIX

static const double Rg = 6360.0;
static const double Rt = 6420.0;
static const double RL = 6421.0;
static const double exposure = 0.4;
static const float ISun = 100.0;

#define RES_MU 128
#define RES_MU_S 32
#define RES_R 32
#define RES_NU 8
#define TRANSMITTANCE_INTEGRAL_SAMPLES 500

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

#define step(_a_,_b_) ((_a_) < (_b_) ? 0 : 1)
#define max(_a_,_b_) ((_a_) < (_b_) ? (_a_) : (_b_))
#define min(_a_,_b_) ((_a_) > (_b_) ? (_a_) : (_b_))
#define sign(_a_) ((_a_) < 0.0 ? -1.0 : ((_a_) > 0.0 ? 1.0 : 0.0))
#define mix(_x_,_y_,_a_) ((_x_) * (1.0 - (_a_)) + (_y_) * (_a_))
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

typedef struct
{
    int xsize;
    int ysize;
    Color* data;
} Texture2D;

typedef struct
{
    int xsize;
    int ysize;
    int zsize;
    Color* data;
} Texture3D;

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
Color getMie(Color rayMie)
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

static Texture3D _precomputeInscatterSampler()
{
    Texture3D result;

    result.xsize = 0;
    result.ysize = 0;
    result.zsize = 0;
    result.data = 0;

    return result;
}
Texture3D _inscatterSampler;

/* nearest intersection of ray r,mu with ground or top atmosphere boundary
 * mu=cos(ray zenith angle at ray origin) */
static double limit(double r, double mu)
{
    double dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
    double delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
    if (delta2 >= 0.0)
    {
        float din = -r * mu - sqrt(delta2);
        if (din >= 0.0) {
            dout = min(dout, din);
        }
    }
    return dout;
}

static double _opticalDepthTransmittance(double H, double r, double mu)
{
    double result = 0.0;
    double dx = limit(r, mu) / (double)TRANSMITTANCE_INTEGRAL_SAMPLES;
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

static Color _debugSave2D(void* data, int x, int y)
{
    Texture2D* tex = (Texture2D*)data;
    return tex->data[y * tex->xsize + x];
}

static Texture2D _precomputeTransmittanceSampler()
{
    Texture2D result;
    int x, y;

    result.xsize = 256;
    result.ysize = 64;
    result.data = malloc(sizeof(Color) * result.xsize * result.ysize); /* TODO free */

    for (x = 0; x < result.xsize; x++)
    {
        for (y = 0; y < result.ysize; y++)
        {
            double r, muS;
            _getTransmittanceRMu((double)x / result.xsize, (double)y / result.ysize, &r, &muS);
            double depth1 = _opticalDepthTransmittance(HR, r, muS);
            double depth2 = _opticalDepthTransmittance(HM, r, muS);
            Color trans;
            trans.r = exp(-(betaR.r * depth1 + betaMEx.x * depth2));
            trans.g = exp(-(betaR.g * depth1 + betaMEx.y * depth2));
            trans.b = exp(-(betaR.b * depth1 + betaMEx.z * depth2));
            trans.a = 1.0;
            result.data[y * result.xsize + x] = trans; /* Eq (5) */
        }
    }

    /* DEBUG */
    systemSavePictureFile("debug.png", _debugSave2D, &result, result.xsize, result.ysize);

    return result;
}
Texture2D _transmittanceSampler;

void brunetonInit()
{
    _inscatterSampler = _precomputeInscatterSampler();
    _transmittanceSampler = _precomputeTransmittanceSampler();

    exit(1);
}

static Color _texture2D(Texture2D* tex, double x, double y)
{
    /* TODO Sampling */
    return COLOR_BLACK;
}

static Color _texture3D(Texture3D* tex, Vector3 location)
{
    /* TODO Sampling */
    return COLOR_BLACK;
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

/* inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0) */
static Color _inscatter(Vector3* _x, double* _t, Vector3 v, Vector3 s, double* _r, double* _mu, Vector3* attenuation)
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
        Color inscatter = vec4max(_texture4D(&_inscatterSampler, r, mu, muS, nu), 0.0);
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
                Color samp = _texture4D(&_inscatterSampler, r0, mu0, muS0, nu);
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
                    Color inScatter0 = _texture4D(&_inscatterSampler, r, mu, muS, nu);
                    Color inScatter1 = _texture4D(&_inscatterSampler, r0, mu0, muS0, nu);
                    Color inScatterA = _applyInscatter(inScatter0, attmod, inScatter1);

                    mu = muHoriz + EPS;
                    r0 = sqrt(r * r + t * t + 2.0 * r * t * mu);
                    mu0 = (r * mu + t) / r0;
                    inScatter0 = _texture4D(&_inscatterSampler, r, mu, muS, nu);
                    inScatter1 = _texture4D(&_inscatterSampler, r0, mu0, muS0, nu);
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
        Color mie = getMie(inscatter);
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

static inline void _getTransmittanceUV(double r, double mu, double* u, double* v)
{
#ifdef TRANSMITTANCE_NON_LINEAR
    *v = sqrt((r - Rg) / (Rt - Rg));
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
    return _texture2D(&_transmittanceSampler, u, v);
}

/* transmittance(=transparency) of atmosphere for infinite ray (r,mu)
   (mu=cos(view zenith angle)), or zero if ray intersects ground */
static Color _transmittanceWithShadow(double r, double mu)
{
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? COLOR_BLACK : _transmittance(r, mu);
}

/* direct sun light for ray x+tv, when sun in direction s (=L0) */
static Color _sunColor(Vector3 x, double t, Vector3 v, Vector3 s, double r, double mu)
{
    if (t > 0.0)
    {
        return COLOR_BLACK;
    }
    else
    {
        Color transmittance = r <= Rt ? _transmittanceWithShadow(r, mu) : COLOR_WHITE; // T(x,xo)
        double isun = step(cos(M_PI / 180.0), v3Dot(v, s)) * ISun; // Lsun
        transmittance.r *= isun;
        transmittance.g *= isun;
        transmittance.b *= isun;
        transmittance.a *= isun;
        return transmittance; // Eq (9)
    }
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
    Color inscatterColor = _inscatter(&x, &t, v, s, &r, &mu, &attenuation); //S[L]-T(x,xs)S[l]|xs
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
