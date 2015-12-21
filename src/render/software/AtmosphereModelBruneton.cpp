#include "AtmosphereModelBruneton.h"

/*
 * Atmospheric scattering, based on E. Bruneton and F.Neyret work.
 * http://evasion.inrialpes.fr/~Eric.Bruneton/
 */

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "System.h"
#include "ParallelWork.h"
#include "PackStream.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereResult.h"
#include "SoftwareRenderer.h"
#include "WaterRenderer.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "Texture2D.h"
#include "Texture4D.h"
#include "CacheFile.h"
#include "FloatNode.h"

/* Factor to convert software units to kilometers */
#define WORLD_SCALING 0.03
#define SUN_DISTANCE 149597870.0
#define SUN_DISTANCE_SCALED (SUN_DISTANCE / WORLD_SCALING)
#define WORKAROUND_OFFSET 0.1

// TODO This is copied in AtmosphereRenderer
#define SPHERE_SIZE 20000.0

/*********************** Constants ***********************/

static const double Rg = 6360.0;
static const double Rt = 6420.0;
static const double RL = 6421.0;
static const double ISun = 100.0;
static const double AVERAGE_GROUND_REFLECTANCE = 0.1;

#if 1
#define RES_MU 128
#define RES_MU_S 32
#define RES_R 32
#define RES_NU 8
#define SKY_W 256
#define SKY_H 64
#define TRANSMITTANCE_W 512
#define TRANSMITTANCE_H 128
#define TRANSMITTANCE_INTEGRAL_SAMPLES 500
#define INSCATTER_INTEGRAL_SAMPLES 100
#define IRRADIANCE_INTEGRAL_SAMPLES 64
#define INSCATTER_SPHERICAL_INTEGRAL_SAMPLES 32
#else
#define RES_MU 64
#define RES_MU_S 16
#define RES_R 16
#define RES_NU 8
#define SKY_W 64
#define SKY_H 16
#define TRANSMITTANCE_W 256
#define TRANSMITTANCE_H 64
#define TRANSMITTANCE_INTEGRAL_SAMPLES 100
#define INSCATTER_INTEGRAL_SAMPLES 10
#define IRRADIANCE_INTEGRAL_SAMPLES 16
#define INSCATTER_SPHERICAL_INTEGRAL_SAMPLES 8
#endif

Texture2D *_transmittanceTexture = NULL;
Texture2D *_irradianceTexture = NULL;
Texture4D *_inscatterTexture = NULL;

/* Rayleigh */
static const double HR = 8.0;
static const Color betaR = {5.8e-3, 1.35e-2, 3.31e-2, 1.0};

/* Mie */
// TODO Use the good ones, determined by weather definition
/* DEFAULT */
/*static const double HM = 1.2;
static const Vector3 betaMSca = {4e-3, 4e-3, 4e-3};
static const Vector3 betaMEx = {4e-3 / 0.9, 4e-3 / 0.9, 4e-3 / 0.9};
static const double mieG = 0.8;*/
/* CLEAR SKY */
static const double HM = 1.2;
static const Vector3 betaMSca = {20e-3, 20e-3, 20e-3};
static const Vector3 betaMEx = {20e-3 / 0.9, 20e-3 / 0.9, 20e-3 / 0.9};
static const double mieG = 0.76;
/* PARTLY CLOUDY */
/*static const double HM = 3.0;
static const Vector3 betaMSca = {3e-3, 3e-3, 3e-3};
static const Vector3 betaMEx = {3e-3 / 0.9, 3e-3 / 0.9, 3e-3 / 0.9};
static const double mieG = 0.65;*/

/*********************** Shader helpers ***********************/

#define step(_a_, _b_) ((_b_) < (_a_) ? 0.0 : 1.0)
#define sign(_a_) ((_a_) < 0.0 ? -1.0 : ((_a_) > 0.0 ? 1.0 : 0.0))
#define mix(_x_, _y_, _a_) ((_x_) * (1.0 - (_a_)) + (_y_) * (_a_))
static inline double min(double a, double b) {
    return a < b ? a : b;
}
static inline double max(double a, double b) {
    return a > b ? a : b;
}
static inline Color vec4mix(Color v1, Color v2, double a) {
    v1.r = mix(v1.r, v2.r, a);
    v1.g = mix(v1.g, v2.g, a);
    v1.b = mix(v1.b, v2.b, a);
    v1.a = mix(v1.a, v2.a, a);
    return v1;
}
static inline double clamp(double x, double minVal, double maxVal) {
    if (x < minVal) {
        x = minVal;
    }
    return (x > maxVal) ? maxVal : x;
}
static inline double smoothstep(double edge0, double edge1, double x) {
    double t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}
static inline void _fixVec4Min(Color *vec, double minVal) {
    if (vec->r < minVal) {
        vec->r = minVal;
    }
    if (vec->g < minVal) {
        vec->g = minVal;
    }
    if (vec->b < minVal) {
        vec->b = minVal;
    }
    if (vec->a < minVal) {
        vec->a = minVal;
    }
}
static inline Color vec4max(Color vec, double minVal) {
    if (vec.r < minVal) {
        vec.r = minVal;
    }
    if (vec.g < minVal) {
        vec.g = minVal;
    }
    if (vec.b < minVal) {
        vec.b = minVal;
    }
    if (vec.a < minVal) {
        vec.a = minVal;
    }
    return vec;
}

static inline Vector3 vec3(double x, double y, double z) {
    Vector3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

static inline Color vec4(double r, double g, double b, double a) {
    Color result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

/*********************** Texture manipulation ***********************/

static Color _texture4D(Texture4D *tex, double r, double mu, double muS, double nu) {
    if (r < Rg + 0.00000001)
        r = Rg + 0.00000001;
    double H = sqrt(Rt * Rt - Rg * Rg);
    double rho = sqrt(r * r - Rg * Rg);
    double rmu = r * mu;
    double delta = rmu * rmu - r * r + Rg * Rg;
    Color cst = (rmu < 0.0 && delta > 0.0) ? vec4(1.0, 0.0, 0.0, 0.5 - 0.5 / to_double(RES_MU))
                                           : vec4(-1.0, H * H, H, 0.5 + 0.5 / to_double(RES_MU));
    double uR = 0.5 / to_double(RES_R) + rho / H * (1.0 - 1.0 / to_double(RES_R));
    double uMu = cst.a + (rmu * cst.r + sqrt(delta + cst.g)) / (rho + cst.b) * (0.5 - 1.0 / to_double(RES_MU));
    double uMuS =
        0.5 / to_double(RES_MU_S) +
        (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / to_double(RES_MU_S));

    return tex->getLinear(uMu, uMuS, nu, uR);
}

/*********************** Physics functions ***********************/

/* Rayleigh phase function */
static double _phaseFunctionR(double mu) {
    return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
}

/* Mie phase function */
static double _phaseFunctionM(double mu) {
    return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG * mieG) * pow(1.0 + (mieG * mieG) - 2.0 * mieG * mu, -3.0 / 2.0) *
           (1.0 + mu * mu) / (2.0 + mieG * mieG);
}

/* approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision") */
static Color _getMie(Color rayMie) {
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
static double _opticalDepth(double H, double r, double mu, double d) {
    double a = sqrt((0.5 / H) * r);
    double ax = a * (mu);
    double ay = a * (mu + d / r);
    double axs = sign(ax);
    double ays = sign(ay);
    double axq = ax * ax;
    double ayq = ay * ay;
    double x = ays > axs ? exp(axq) : 0.0;
    double yx = axs / (2.3193 * fabs(ax) + sqrt(1.52 * axq + 4.0));
    double yy = ays / (2.3193 * fabs(ay) + sqrt(1.52 * ayq + 4.0)) * exp(-d / H * (d / (2.0 * r) + mu));
    return sqrt((6.2831 * H) * r) * exp((Rg - r) / H) * (x + yx - yy);
}

static inline void _getTransmittanceUV(double r, double mu, double *u, double *v) {
    if (r < Rg + 0.00000001)
        r = Rg + 0.00000001;
    double dr = (r - Rg) / (Rt - Rg);
    *v = sqrt(dr);
    *u = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;
}

/* transmittance(=transparency) of atmosphere for infinite ray (r,mu)
   (mu=cos(view zenith angle)), intersections with ground ignored */
static Color _transmittance(double r, double mu) {
    double u, v;
    _getTransmittanceUV(r, mu, &u, &v);
    return _transmittanceTexture->getLinear(u, v);
}

/* transmittance(=transparency) of atmosphere between x and x0
 * assume segment x,x0 not intersecting ground
 * d = distance between x and x0, mu=cos(zenith angle of [x,x0) ray at x) */
static Color _transmittance3(double r, double mu, double d) {
    Color result, t1, t2;
    double r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);
    double mu1 = (r * mu + d) / r1;
    if (mu > 0.0) {
        t1 = _transmittance(r, mu);
        t2 = _transmittance(r1, mu1);
    } else {
        t1 = _transmittance(r1, -mu1);
        t2 = _transmittance(r, -mu);
    }
    result.r = min(t1.r / t2.r, 1.0);
    result.g = min(t1.g / t2.g, 1.0);
    result.b = min(t1.b / t2.b, 1.0);
    result.a = 1.0;
    return result;
}

static void _getIrradianceRMuS(double x, double y, double *r, double *muS) {
    *r = Rg + y *(Rt - Rg);
    *muS = -0.2 + x *(1.0 + 0.2);
}

/* nearest intersection of ray r,mu with ground or top atmosphere boundary
 * mu=cos(ray zenith angle at ray origin) */
static double _limit(double r, double mu) {
    double dout = -r * mu + sqrt(r * r * (mu * mu - 1.0) + RL * RL);
    double delta2 = r * r * (mu * mu - 1.0) + Rg * Rg;
    if (delta2 >= 0.0) {
        double din = -r * mu - sqrt(delta2);
        if (din >= 0.0) {
            dout = min(dout, din);
        }
    }
    return dout;
}

/* transmittance(=transparency) of atmosphere for ray (r,mu) of length d
   (mu=cos(view zenith angle)), intersections with ground ignored
   uses analytic formula instead of transmittance texture */
static Vector3 _analyticTransmittance(double r, double mu, double d) {
    Vector3 result;

    result.x = exp(-betaR.r * _opticalDepth(HR, r, mu, d) - betaMEx.x * _opticalDepth(HM, r, mu, d));
    result.y = exp(-betaR.g * _opticalDepth(HR, r, mu, d) - betaMEx.y * _opticalDepth(HM, r, mu, d));
    result.z = exp(-betaR.b * _opticalDepth(HR, r, mu, d) - betaMEx.z * _opticalDepth(HM, r, mu, d));

    return result;
}

/* transmittance(=transparency) of atmosphere for infinite ray (r,mu)
   (mu=cos(view zenith angle)), or zero if ray intersects ground */
static Color _transmittanceWithShadow(double r, double mu) {
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? COLOR_BLACK : _transmittance(r, mu);
}

static void _texCoordToMuMuSNu(double x, double y, double z, double r, Color dhdH, double *mu, double *muS,
                               double *nu) {
    double d;

    x /= to_double(RES_MU);
    y /= to_double(RES_MU_S);
    z /= to_double(RES_NU);

    if (x < 0.5) {
        d = 1.0 - x / 0.5;
        d = min(max(dhdH.b, d * dhdH.a), dhdH.a * 0.999);
        *mu = (Rg * Rg - r * r - d * d) / (2.0 * r * d);
        *mu = min(*mu, -sqrt(1.0 - (Rg / r) * (Rg / r)) - 0.001);
    } else {
        d = (x - 0.5) / 0.5;
        d = min(max(dhdH.r, d * dhdH.g), dhdH.g * 0.999);
        *mu = (Rt * Rt - r * r - d * d) / (2.0 * r * d);
    }
    *muS = tan((2.0 * y - 1.0 + 0.26) * 1.1) / tan(1.26 * 1.1);
    *nu = -1.0 + z / 2.0;
}

static void _getIrradianceUV(double r, double muS, double *uMuS, double *uR) {
    *uR = (r - Rg) / (Rt - Rg);
    *uMuS = (muS + 0.2) / (1.0 + 0.2);
}

static Color _irradiance(Texture2D *sampler, double r, double muS) {
    double u, v;
    _getIrradianceUV(r, muS, &u, &v);
    return sampler->getLinear(u, v);
}

/*********************** transmittance.glsl ***********************/

static void _getTransmittanceRMu(double x, double y, double *r, double *muS) {
    *r = Rg + (y * y) * (Rt - Rg);
    *muS = -0.15 + tan(1.5 * x) / tan(1.5) * (1.0 + 0.15);
}

static double _opticalDepthTransmittance(double H, double r, double mu) {
    double result = 0.0;
    double dx = _limit(r, mu) / to_double(TRANSMITTANCE_INTEGRAL_SAMPLES);
    double yi = exp(-(r - Rg) / H);
    int i;
    for (i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i) {
        double xj = to_double(i) * dx;
        double yj = exp(-(sqrt(r * r + xj * xj + 2.0 * xj * r * mu) - Rg) / H);
        result += (yi + yj) / 2.0 * dx;
        yi = yj;
    }
    return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 1e9 : result;
}

static void _precomputeTransmittanceTexture() {
    int x, y;

    for (x = 0; x < TRANSMITTANCE_W; x++) {
        for (y = 0; y < TRANSMITTANCE_H; y++) {
            double r, muS;
            _getTransmittanceRMu(to_double(x + 0.5) / TRANSMITTANCE_W, to_double(y + 0.5) / TRANSMITTANCE_H, &r, &muS);
            double depth1 = _opticalDepthTransmittance(HR, r, muS);
            double depth2 = _opticalDepthTransmittance(HM, r, muS);
            Color trans;
            trans.r = exp(-(betaR.r * depth1 + betaMEx.x * depth2));
            trans.g = exp(-(betaR.g * depth1 + betaMEx.y * depth2));
            trans.b = exp(-(betaR.b * depth1 + betaMEx.z * depth2));
            trans.a = 1.0;
            _transmittanceTexture->setPixel(x, y, trans); /* Eq (5) */
        }
    }
}

/*********************** irradiance1.glsl ***********************/

static void _precomputeIrrDeltaETexture(Texture2D *destination) {
    int x, y;

    /* Irradiance program */
    for (x = 0; x < SKY_W; x++) {
        for (y = 0; y < SKY_H; y++) {
            double r, muS;
            Color trans, irr;
            _getIrradianceRMuS(to_double(x) / SKY_W, to_double(y) / SKY_H, &r, &muS);
            trans = _transmittance(r, muS);

            irr.r = trans.r * max(muS, 0.0);
            irr.g = trans.g * max(muS, 0.0);
            irr.b = trans.b * max(muS, 0.0);
            irr.a = 1.0;

            destination->setPixel(x, y, irr);
        }
    }
}

static void _getLayerParams(int layer, double *_r, Color *_dhdH) {
    double r = layer / (RES_R - 1.0);
    r = r * r;
    r = sqrt(Rg * Rg + r * (Rt * Rt - Rg * Rg)) + (layer == 0 ? 0.01 : (layer == RES_R - 1 ? -0.001 : 0.0));
    double dmin = Rt - r;
    double dmax = sqrt(r * r - Rg * Rg) + sqrt(Rt * Rt - Rg * Rg);
    double dminp = r - Rg;
    double dmaxp = sqrt(r * r - Rg * Rg);

    *_r = r;
    _dhdH->r = dmin;
    _dhdH->g = dmax;
    _dhdH->b = dminp;
    _dhdH->a = dmaxp;
}

/*********************** inscatter1.glsl ***********************/

static void _integrand1(double r, double mu, double muS, double nu, double t, Color *ray, Color *mie) {
    double ri = sqrt(r * r + t * t + 2.0 * r * mu * t);
    double muSi = (nu * t + muS * r) / ri;
    ri = max(Rg, ri);
    if (muSi >= -sqrt(1.0 - Rg * Rg / (ri * ri))) {
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
    } else {
        ray->r = ray->g = ray->b = 0.0;
        mie->r = mie->g = mie->b = 0.0;
    }
}

static void _inscatter1(double r, double mu, double muS, double nu, Color *ray, Color *mie) {
    ray->r = ray->g = ray->b = 0.0;
    mie->r = mie->g = mie->b = 0.0;
    double dx = _limit(r, mu) / to_double(INSCATTER_INTEGRAL_SAMPLES);
    Color rayi;
    Color miei;
    _integrand1(r, mu, muS, nu, 0.0, &rayi, &miei);
    int i;
    for (i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i) {
        double xj = to_double(i)*dx;
        Color rayj;
        Color miej;
        _integrand1(r, mu, muS, nu, xj, &rayj, &miej);
        ray->r += (rayi.r + rayj.r) / 2.0 * dx;
        ray->g += (rayi.g + rayj.g) / 2.0 * dx;
        ray->b += (rayi.b + rayj.b) / 2.0 * dx;
        mie->r += (miei.r + miej.r) / 2.0 * dx;
        mie->g += (miei.g + miej.g) / 2.0 * dx;
        mie->b += (miei.b + miej.b) / 2.0 * dx;
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

typedef struct {
    Texture4D *ray;
    Texture4D *mie;
} Inscatter1Params;

static int _inscatter1Worker(ParallelWork *, int layer, void *data) {
    Inscatter1Params *params = (Inscatter1Params *)data;

    double r;
    Color dhdH;
    _getLayerParams(layer, &r, &dhdH);

    int x, y, z;
    for (x = 0; x < RES_MU; x++) {
        for (y = 0; y < RES_MU_S; y++) {
            for (z = 0; z < RES_NU; z++) {
                Color ray = COLOR_BLACK;
                Color mie = COLOR_BLACK;
                double mu, muS, nu;
                _texCoordToMuMuSNu(to_double(x), to_double(y), to_double(z), r, dhdH, &mu, &muS, &nu);
                _inscatter1(r, mu, muS, nu, &ray, &mie);
                /* store separately Rayleigh and Mie contributions, WITHOUT the phase function factor
                 * (cf "Angular precision") */
                params->ray->setPixel(x, y, z, layer, ray);
                params->mie->setPixel(x, y, z, layer, mie);
            }
        }
    }
    return 1;
}

/*********************** inscatterS.glsl ***********************/

static Color _inscatterS(double r, double mu, double muS, double nu, int first, Texture2D *deltaE, Texture4D *deltaSR,
                         Texture4D *deltaSM) {
    Color raymie = COLOR_BLACK;

    double dphi = M_PI / to_double(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);
    double dtheta = M_PI / to_double(INSCATTER_SPHERICAL_INTEGRAL_SAMPLES);

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
    for (itheta = 0; itheta < INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++itheta) {
        double theta = (to_double(itheta) + 0.5) * dtheta;
        double ctheta = cos(theta);

        double greflectance = 0.0;
        double dground = 0.0;
        Color gtransp = {0.0, 0.0, 0.0, 0.0};
        if (ctheta < cthetamin) {
            /* if ground visible in direction w
             * compute transparency gtransp between x and ground */
            greflectance = AVERAGE_GROUND_REFLECTANCE / M_PI;
            dground = -r * ctheta - sqrt(r * r * (ctheta * ctheta - 1.0) + Rg * Rg);
            gtransp = _transmittance3(Rg, -(r * ctheta + dground) / Rg, dground);
        }

        int iphi;
        for (iphi = 0; iphi < 2 * INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++iphi) {
            double phi = (to_double(iphi) + 0.5) * dphi;
            double dw = dtheta * dphi * sin(theta);
            Vector3 w = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), ctheta);

            double nu1 = s.dotProduct(w);
            double nu2 = v.dotProduct(w);
            double pr2 = _phaseFunctionR(nu2);
            double pm2 = _phaseFunctionM(nu2);

            /* compute irradiance received at ground in direction w (if ground visible) =deltaE */
            Vector3 gnormal;
            gnormal.x = dground * w.x / Rg;
            gnormal.y = dground * w.y / Rg;
            gnormal.z = (r + dground * w.z) / Rg;
            Color girradiance = _irradiance(deltaE, Rg, gnormal.dotProduct(s));

            Color raymie1; /* light arriving at x from direction w */

            /* first term = light reflected from the ground and attenuated before reaching x, =T.alpha/PI.deltaE */
            raymie1.r = greflectance * girradiance.r * gtransp.r;
            raymie1.g = greflectance * girradiance.g * gtransp.g;
            raymie1.b = greflectance * girradiance.b * gtransp.b;

            /* second term = inscattered light, =deltaS */
            if (first) {
                /* first iteration is special because Rayleigh and Mie were stored separately,
                 * without the phase functions factors; they must be reintroduced here */
                double pr1 = _phaseFunctionR(nu1);
                double pm1 = _phaseFunctionM(nu1);
                Color ray1 = _texture4D(deltaSR, r, w.z, muS, nu1);
                Color mie1 = _texture4D(deltaSM, r, w.z, muS, nu1);
                raymie1.r += ray1.r * pr1 + mie1.r * pm1;
                raymie1.g += ray1.g * pr1 + mie1.g * pm1;
                raymie1.b += ray1.b * pr1 + mie1.b * pm1;
            } else {
                Color col = _texture4D(deltaSR, r, w.z, muS, nu1);
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

typedef struct {
    Texture4D *result;
    Texture2D *deltaE;
    Texture4D *deltaSR;
    Texture4D *deltaSM;
    int first;
} jParams;

static int _jWorker(ParallelWork *, int layer, void *data) {
    jParams *params = (jParams *)data;

    double r;
    Color dhdH;
    _getLayerParams(layer, &r, &dhdH);

    int x, y, z;
    for (x = 0; x < RES_MU; x++) {
        for (y = 0; y < RES_MU_S; y++) {
            for (z = 0; z < RES_NU; z++) {
                Color raymie;
                double mu, muS, nu;
                _texCoordToMuMuSNu(to_double(x), to_double(y), to_double(z), r, dhdH, &mu, &muS, &nu);
                raymie = _inscatterS(r, mu, muS, nu, params->first, params->deltaE, params->deltaSR, params->deltaSM);
                params->result->setPixel(x, y, z, layer, raymie);
            }
        }
    }
    return 1;
}

/*********************** irradianceN.glsl ***********************/

void _irradianceNProg(Texture2D *destination, Texture4D *deltaSR, Texture4D *deltaSM, int first) {
    int x, y;
    double dphi = M_PI / to_double(IRRADIANCE_INTEGRAL_SAMPLES);
    double dtheta = M_PI / to_double(IRRADIANCE_INTEGRAL_SAMPLES);
    for (x = 0; x < SKY_W; x++) {
        for (y = 0; y < SKY_H; y++) {
            double r, muS;
            int iphi;
            _getIrradianceRMuS(to_double(x) / SKY_W, to_double(y) / SKY_H, &r, &muS);
            Vector3 s = vec3(max(sqrt(1.0 - muS * muS), 0.0), 0.0, muS);

            Color result = COLOR_BLACK;
            /* integral over 2.PI around x with two nested loops over w directions (theta,phi) -- Eq (15) */
            for (iphi = 0; iphi < 2 * IRRADIANCE_INTEGRAL_SAMPLES; ++iphi) {
                double phi = (to_double(iphi) + 0.5) * dphi;
                int itheta;
                for (itheta = 0; itheta < IRRADIANCE_INTEGRAL_SAMPLES / 2; ++itheta) {
                    double theta = (to_double(itheta) + 0.5) * dtheta;
                    double dw = dtheta * dphi * sin(theta);
                    Vector3 w = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
                    double nu = s.dotProduct(w);
                    if (first) {
                        /* first iteration is special because Rayleigh and Mie were stored separately,
                           without the phase functions factors; they must be reintroduced here */
                        double pr1 = _phaseFunctionR(nu);
                        double pm1 = _phaseFunctionM(nu);
                        Color ray1 = _texture4D(deltaSR, r, w.z, muS, nu);
                        Color mie1 = _texture4D(deltaSM, r, w.z, muS, nu);
                        result.r += (ray1.r * pr1 + mie1.r * pm1) * w.z * dw;
                        result.g += (ray1.g * pr1 + mie1.g * pm1) * w.z * dw;
                        result.b += (ray1.b * pr1 + mie1.b * pm1) * w.z * dw;
                    } else {
                        Color col = _texture4D(deltaSR, r, w.z, muS, nu);
                        result.r += col.r * w.z * dw;
                        result.g += col.g * w.z * dw;
                        result.b += col.b * w.z * dw;
                    }
                }
            }

            destination->setPixel(x, y, result);
        }
    }
}

/*********************** inscatterN.glsl ***********************/

typedef struct {
    Texture4D *destination;
    Texture4D *deltaJ;
} InscatterNParams;

static Color _integrand2(Texture4D *deltaJ, double r, double mu, double muS, double nu, double t) {
    double ri = sqrt(r * r + t * t + 2.0 * r * mu * t);
    double mui = (r * mu + t) / ri;
    double muSi = (nu * t + muS * r) / ri;
    Color c1, c2;
    c1 = _texture4D(deltaJ, ri, mui, muSi, nu);
    c2 = _transmittance3(r, mu, t);
    c1.r *= c2.r;
    c1.g *= c2.g;
    c1.b *= c2.b;
    c1.a = 1.0;
    return c1;
}

static Color _inscatterN(Texture4D *deltaJ, double r, double mu, double muS, double nu) {
    Color raymie = COLOR_BLACK;
    double dx = _limit(r, mu) / to_double(INSCATTER_INTEGRAL_SAMPLES);
    Color raymiei = _integrand2(deltaJ, r, mu, muS, nu, 0.0);
    int i;
    for (i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i) {
        double xj = to_double(i)*dx;
        Color raymiej = _integrand2(deltaJ, r, mu, muS, nu, xj);
        raymie.r += (raymiei.r + raymiej.r) / 2.0 * dx;
        raymie.g += (raymiei.g + raymiej.g) / 2.0 * dx;
        raymie.b += (raymiei.b + raymiej.b) / 2.0 * dx;
        raymiei = raymiej;
    }
    return raymie;
}

static int _inscatterNWorker(ParallelWork *, int layer, void *data) {
    InscatterNParams *params = (InscatterNParams *)data;

    double r;
    Color dhdH;
    _getLayerParams(layer, &r, &dhdH);

    int x, y, z;
    for (x = 0; x < RES_MU; x++) {
        for (y = 0; y < RES_MU_S; y++) {
            for (z = 0; z < RES_NU; z++) {
                double mu, muS, nu;
                _texCoordToMuMuSNu(to_double(x), to_double(y), to_double(z), r, dhdH, &mu, &muS, &nu);
                params->destination->setPixel(x, y, z, layer, _inscatterN(params->deltaJ, r, mu, muS, nu));
            }
        }
    }
    return 1;
}

/*********************** copyInscatterN.glsl ***********************/

typedef struct {
    Texture4D *source;
    Texture4D *destination;
} CopyInscatterNParams;

static int _copyInscatterNWorker(ParallelWork *, int layer, void *data) {
    CopyInscatterNParams *params = (CopyInscatterNParams *)data;

    double r;
    Color dhdH;
    _getLayerParams(layer, &r, &dhdH);

    int x, y, z;
    for (x = 0; x < RES_MU; x++) {
        for (y = 0; y < RES_MU_S; y++) {
            for (z = 0; z < RES_NU; z++) {
                double mu, muS, nu;
                _texCoordToMuMuSNu(to_double(x), to_double(y), to_double(z), r, dhdH, &mu, &muS, &nu);
                Color col1 = params->source->getPixel(x, y, z, layer);
                Color col2 = params->destination->getPixel(x, y, z, layer);
                col2.r += col1.r / _phaseFunctionR(nu);
                col2.g += col1.g / _phaseFunctionR(nu);
                col2.b += col1.b / _phaseFunctionR(nu);
                params->destination->setPixel(x, y, z, layer, col2);
            }
        }
    }
    return 1;
}

/*********************** Final getters ***********************/

static inline Color _applyInscatter(Color inscatter, Color attmod, Color samp) {
    inscatter.r = inscatter.r - attmod.r * samp.r;
    inscatter.g = inscatter.g - attmod.g * samp.g;
    inscatter.b = inscatter.b - attmod.b * samp.b;
    inscatter.a = inscatter.a - attmod.a * samp.a;
    return vec4max(inscatter, 0.0);
}

/* inscattered light along ray x+tv, when sun in direction s (=S[L]-T(x,x0)S[L]|x0) */
static Color _getInscatterColor(Vector3 *_x, double *_t, Vector3 v, Vector3 s, double *_r, double *_mu,
                                Vector3 *attenuation) {
    Color result;
    double r = _x->getNorm();
    double mu = _x->dotProduct(v) / r;
    double d = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rt * Rt);
    attenuation->x = attenuation->y = attenuation->z = 0.0;
    if (d > 0.0) {
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
    if (r <= Rt) {
        /* if ray intersects atmosphere */
        double nu = v.dotProduct(s);
        double muS = x.dotProduct(s) / r;
        double phaseR = _phaseFunctionR(nu);
        double phaseM = _phaseFunctionM(nu);
        Color inscatter = vec4max(_texture4D(_inscatterTexture, r, mu, muS, nu), 0.0);
        if (t > 0.0) {
            Vector3 x0 = x.add(v.scale(t));
            double r0 = x0.getNorm();
            double rMu0 = x0.dotProduct(v);
            double mu0 = rMu0 / r0;
            double muS0 = x0.dotProduct(s) / r0;
            /* avoids imprecision problems in transmittance computations based on textures */
            *attenuation = _analyticTransmittance(r, mu, t);
            if (r0 > Rg + 0.01) {
                /* computes S[L]-T(x,x0)S[L]|x0 */
                Color attmod = {attenuation->x, attenuation->y, attenuation->z, attenuation->x};
                Color samp = _texture4D(_inscatterTexture, r0, mu0, muS0, nu);
                inscatter = _applyInscatter(inscatter, attmod, samp);
                /* avoids imprecision problems near horizon by interpolating between two points above and below horizon
                 */
                const double EPS = 0.004;
                double muHoriz = -sqrt(1.0 - (Rg / r) * (Rg / r));
                if (fabs(mu - muHoriz) < EPS) {
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
            }
        }
        /* avoids imprecision problems in Mie scattering when sun is below horizon */
        inscatter.a *= smoothstep(0.00, 0.02, muS);
        Color mie = _getMie(inscatter);
        result.r = inscatter.r * phaseR + mie.r * phaseM;
        result.g = inscatter.g * phaseR + mie.g * phaseM;
        result.b = inscatter.b * phaseR + mie.b * phaseM;
        result.a = 1.0;
        _fixVec4Min(&result, 0.0);
    } else {
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

/* direct sun light for ray x+tv, when sun in direction s (=L0) */
static Color _sunColor(Vector3 v, Vector3 s, double r, double mu, double radius) {
    Color transmittance = r <= Rt ? _transmittanceWithShadow(r, mu) : COLOR_WHITE; /* T(x,xo) */
    double d = _limit(r, mu);
    radius *= (1.0 + 25.0 * d / Rt); /* Inflating due to lens effect near horizon */
    double isun = step(cos(radius * M_PI / 180.0), v.dotProduct(s)) * ISun; /* Lsun */
    transmittance.r *= isun;
    transmittance.g *= isun;
    transmittance.b *= isun;
    transmittance.a = 1.0;
    return transmittance; /* Eq (9) */
}

/*********************** Cache/debug methods ***********************/

static int _tryLoadCache2D(Texture2D *tex, const char *tag, int order) {
    int xsize, ysize;
    tex->getSize(&xsize, &ysize);

    CacheFile cache("atmo-br", "cache", tag, xsize, ysize, 0, 0, order);
    if (cache.isReadable()) {
        PackStream stream;
        stream.bindToFile(cache.getPath());
        tex->load(&stream);

        return 1;
    } else {
        return 0;
    }
}

static void _saveCache2D(Texture2D *tex, const char *tag, int order) {
    int xsize, ysize;
    tex->getSize(&xsize, &ysize);

    CacheFile cache("atmo-br", "cache", tag, xsize, ysize, 0, 0, order);
    if (cache.isWritable()) {
        PackStream stream;
        stream.bindToFile(cache.getPath(), true);
        tex->save(&stream);
    }
}

static void _saveDebug2D(Texture2D *tex, const char *tag, int order) {
    int xsize, ysize;
    tex->getSize(&xsize, &ysize);

    CacheFile cache("atmo-br", "png", tag, xsize, ysize, 0, 0, order);
    if (cache.isWritable()) {
        tex->saveToFile(cache.getPath());
    }
}

static int _tryLoadCache4D(Texture4D *tex, const char *tag, int order) {
    int xsize, ysize, zsize, wsize;
    tex->getSize(&xsize, &ysize, &zsize, &wsize);

    CacheFile cache("atmo-br", "cache", tag, xsize, ysize, zsize, wsize, order);
    if (cache.isReadable()) {
        PackStream stream;
        stream.bindToFile(cache.getPath());
        tex->load(&stream);

        return 1;
    } else {
        return 0;
    }
}

static void _saveCache4D(Texture4D *tex, const char *tag, int order) {
    int xsize, ysize, zsize, wsize;
    tex->getSize(&xsize, &ysize, &zsize, &wsize);

    CacheFile cache("atmo-br", "cache", tag, xsize, ysize, zsize, wsize, order);
    if (cache.isWritable()) {
        PackStream stream;
        stream.bindToFile(cache.getPath(), true);
        tex->save(&stream);
    }
}

static void _saveDebug4D(Texture4D *tex, const char *tag, int order) {
    int xsize, ysize, zsize, wsize;
    tex->getSize(&xsize, &ysize, &zsize, &wsize);

    CacheFile cache("atmo-br", "png", tag, xsize, ysize, zsize, wsize, order);
    if (cache.isWritable()) {
        tex->saveToFile(cache.getPath());
    }
}

/*********************** Public methods ***********************/
int brunetonInit() {
    int x, y, z, w, order;
    ParallelWork *work;

    assert(_inscatterTexture == NULL);

    /* TODO Deletes */
    _transmittanceTexture = new Texture2D(TRANSMITTANCE_W, TRANSMITTANCE_H);
    _irradianceTexture = new Texture2D(SKY_W, SKY_H);
    _inscatterTexture = new Texture4D(RES_MU, RES_MU_S, RES_NU, RES_R);

    /* try loading from cache */
    if (_tryLoadCache2D(_transmittanceTexture, "transmittance", 0) &&
        _tryLoadCache2D(_irradianceTexture, "irradiance", 0) && _tryLoadCache4D(_inscatterTexture, "inscatter", 0)) {
        return 1;
    }

    Texture2D *_deltaETexture = new Texture2D(SKY_W, SKY_H);
    Texture4D *_deltaSMTexture = new Texture4D(RES_MU, RES_MU_S, RES_NU, RES_R);
    Texture4D *_deltaSRTexture = new Texture4D(RES_MU, RES_MU_S, RES_NU, RES_R);
    Texture4D *_deltaJTexture = new Texture4D(RES_MU, RES_MU_S, RES_NU, RES_R);

    /* computes transmittance texture T (line 1 in algorithm 4.1) */
    _precomputeTransmittanceTexture();
    _saveDebug2D(_transmittanceTexture, "transmittance", 0);

    /* computes irradiance texture deltaE (line 2 in algorithm 4.1) */
    _precomputeIrrDeltaETexture(_deltaETexture);
    _saveDebug2D(_deltaETexture, "deltaE", 0);

    /* computes single scattering texture deltaS (line 3 in algorithm 4.1)
     * Rayleigh and Mie separated in deltaSR + deltaSM */
    Inscatter1Params params = {_deltaSRTexture, _deltaSMTexture};
    work = new ParallelWork(_inscatter1Worker, RES_R, &params);
    work->perform();
    delete work;
    _saveDebug4D(_deltaSRTexture, "deltaSR", 0);
    _saveDebug4D(_deltaSMTexture, "deltaSM", 0);

    /* copies deltaE into irradiance texture E (line 4 in algorithm 4.1) */
    /* ??? all black texture (k=0.0) ??? */
    _irradianceTexture->fill(COLOR_BLACK);

    /* copies deltaS into inscatter texture S (line 5 in algorithm 4.1) */
    for (x = 0; x < RES_MU; x++) {
        for (y = 0; y < RES_MU_S; y++) {
            for (z = 0; z < RES_NU; z++) {
                for (w = 0; w < RES_R; w++) {
                    Color result = _deltaSRTexture->getPixel(x, y, z, w);
                    Color mie = _deltaSMTexture->getPixel(x, y, z, w);
                    result.a = mie.r;
                    _inscatterTexture->setPixel(x, y, z, w, result);
                }
            }
        }
    }
    _saveDebug4D(_inscatterTexture, "inscatter", 0);

    /* loop for each scattering order (line 6 in algorithm 4.1) */
    for (order = 2; order <= 4; ++order) {
        /* computes deltaJ (line 7 in algorithm 4.1) */
        jParams jparams = {_deltaJTexture, _deltaETexture, _deltaSRTexture, _deltaSMTexture, order == 2};
        work = new ParallelWork(_jWorker, RES_R, &jparams);
        work->perform();
        delete work;
        _saveDebug4D(_deltaJTexture, "deltaJ", order);

        /* computes deltaE (line 8 in algorithm 4.1) */
        _irradianceNProg(_deltaETexture, _deltaSRTexture, _deltaSMTexture, order == 2);
        _saveDebug2D(_deltaETexture, "deltaE", order);

        /* computes deltaS (line 9 in algorithm 4.1) */
        InscatterNParams iparams = {_deltaSRTexture, _deltaJTexture};
        work = new ParallelWork(_inscatterNWorker, RES_R, &iparams);
        work->perform();
        delete work;

        /* adds deltaE into irradiance texture E (line 10 in algorithm 4.1) */
        _irradianceTexture->add(_deltaETexture);
        _saveDebug2D(_irradianceTexture, "irradiance", order);

        /* adds deltaS into inscatter texture S (line 11 in algorithm 4.1) */
        CopyInscatterNParams cparams = {_deltaSRTexture, _inscatterTexture};
        work = new ParallelWork(_copyInscatterNWorker, RES_R, &cparams);
        work->perform();
        delete work;
        _saveDebug4D(_inscatterTexture, "inscatter", order);
    }

    _saveCache2D(_transmittanceTexture, "transmittance", 0);
    _saveCache2D(_irradianceTexture, "irradiance", 0);
    _saveCache4D(_inscatterTexture, "inscatter", 0);

    delete _deltaETexture;
    delete _deltaSMTexture;
    delete _deltaSRTexture;
    delete _deltaJTexture;

    return 1;
}

static int _inited;

AtmosphereModelBruneton::AtmosphereModelBruneton(SoftwareRenderer *parent) : parent(parent) {
    if (not _inited) {
        _inited = brunetonInit();
    }
}

AtmosphereModelBruneton::~AtmosphereModelBruneton() {
}

AtmosphereResult AtmosphereModelBruneton::getSkyColor(Vector3 eye, const Vector3 &direction,
                                                      const Vector3 &sun_position, const Color &base) {
    Vector3 x = {0.0, Rg + eye.y * WORLD_SCALING, 0.0};
    Vector3 v = direction.normalize();
    Vector3 s = sun_position.sub(x).normalize();

    double r = x.getNorm();
    double mu = x.dotProduct(v) / r;
    double t = -r * mu - sqrt(r * r * (mu * mu - 1.0) + Rg * Rg);

    AtmosphereResult result;
    Vector3 attenuation;
    Color sunColor =
        _sunColor(v, s, r, mu, parent->getScenery()->getAtmosphere()->propSunRadius()->getValue()); /* L0 */

    /*result.base.r = base.r + sunColor.r;
    result.base.g = base.g + sunColor.g;
    result.base.b = base.b + sunColor.b;*/
    result.base = base.add(sunColor);
    result.inscattering = _getInscatterColor(&x, &t, v, s, &r, &mu, &attenuation); /* S[L]-T(x,xs)S[l]|xs */
    /* TODO Use atmosphere attenuation */
    result.distance = SPHERE_SIZE;

    result.updateFinal();

    return result;
}

AtmosphereResult AtmosphereModelBruneton::applyAerialPerspective(Vector3 location, const Color &base) {
    Vector3 eye = parent->getCameraLocation(location);
    eye.y = max(eye.y, 0.0);
    location.y = max(location.y, 0.0);
    Vector3 sun_position = parent->getAtmosphereRenderer()->getSunDirection().scale(SUN_DISTANCE);

    Vector3 direction = location.sub(eye).scale(WORLD_SCALING);
    double t = direction.getNorm();
    if (t < 0.000001) {
        direction = parent->getCameraDirection(location).scale(0.001 * WORLD_SCALING);
        t = direction.getNorm();
    }

    Vector3 x = {0.0, Rg + WORKAROUND_OFFSET + eye.y * WORLD_SCALING, 0.0};
    Vector3 v = direction.normalize();
    Vector3 s = sun_position.sub(x).normalize();

    if (v.y > s.y) {
        v.y = s.y;
        v.normalize();
    }

    if (v.y == 0.0) {
        v.y = -0.000001;
    }

    double r = x.getNorm();
    double mu = x.dotProduct(v) / r;

    AtmosphereResult result;
    Vector3 attenuation;

    result.base = base;
    result.inscattering = _getInscatterColor(&x, &t, v, s, &r, &mu, &attenuation); /* S[L]-T(x,xs)S[l]|xs */
    result.attenuation.r = attenuation.x;
    result.attenuation.g = attenuation.y;
    result.attenuation.b = attenuation.z;
    result.distance = t / WORLD_SCALING;

    result.updateFinal();

    return result;
}

bool AtmosphereModelBruneton::getLightsAt(vector<LightComponent> &result, const Vector3 &location) const {
    LightComponent sun, irradiance;
    double muS;

    double altitude = max(location.y * WORLD_SCALING, 0.0);

    double r0 = Rg + WORKAROUND_OFFSET + altitude;
    Vector3 up = {0.0, 1.0, 0.0};
    Vector3 sun_position = parent->getAtmosphereRenderer()->getSunDirection().scale(SUN_DISTANCE);
    Vector3 x = {0.0, r0, 0.0};
    Vector3 s = sun_position.sub(x).normalize();

    muS = up.dotProduct(s);
    if (altitude > RL) {
        sun.color = parent->getScenery()->getAtmosphere()->sun_color;
    } else {
        sun.color = _transmittanceWithShadow(r0, muS);
    }
    sun.direction = s.scale(-1.0);
    sun.reflection = ISun;
    sun.altered = 1;

    result.push_back(sun);

    irradiance.color = _irradiance(_irradianceTexture, r0, muS);
    irradiance.direction = VECTOR_DOWN;
    irradiance.reflection = 0.0;
    irradiance.altered = 0;

    result.push_back(irradiance);

    return true;
}

Texture2D *AtmosphereModelBruneton::getTextureTransmittance() const {
    return _transmittanceTexture;
}

Texture2D *AtmosphereModelBruneton::getTextureIrradiance() const {
    return _irradianceTexture;
}

Texture4D *AtmosphereModelBruneton::getTextureInscatter() const {
    return _inscatterTexture;
}
