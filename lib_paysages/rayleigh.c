#include "rayleigh.h"

#include <math.h>

static Vector3 _betaR = {5.5e-6, 13.0e-6, 22.4e-6};  /* Rayleigh scattering coefficients at sea level */
static Vector3 _betaM = {21e-6, 0.0, 0.0};           /* Mie scattering coefficients at sea level */
static double _Hr = 7994;                            /* Rayleigh scale height */
static double _Hm = 1200;                            /* Mie scale height */
static double _radiusEarth = 6360e3;                 /* Earth radius */
static double _radiusAtmosphere = 6420e3;            /* Atmosphere radius */
static double _sunIntensity = 20.0;                  /* Sun intensity */
static double _g = 0.76;                             /* Mean cosine */

/*typedef struct
{
    double tmin;
    double tmax;
} Ray;

static Vector3 _computeIncidentLight(Ray r)
{
    double t0, t1;
    int numSamples = 16;
    int numSamplesLight = 8;
    int i, j;
    
    if (!intersect<T>(r, radiusAtmosphere, t0, t1) || t1 < 0) return Vec3<T>(0);
    if (t0 > r.tmin && t0 > 0) r.tmin = t0;
    if (t1 < r.tmax) r.tmax = t1;
    double segmentLength = (r.tmax - r.tmin) / (double)numSamples;
    double tCurrent = r.tmin;
    Vector3 sumR = VECTOR_ZERO;
    Vector3 sumM = VECTOR_ZERO;
    double opticalDepthR = 0.0;
    double opticalDepthM = 0.0;
    double mu = r.direction.dot(sunDirection);
    double phaseR = 3.0 / (16.0 * M_PI) * (1.0 + mu * mu);
    double phaseM = 3.0 / (8.0 * M_PI) * ((1.0 - _g * _g) * (1 + mu * mu)) / ((2 + _g * _g) * pow(1 + _g * _g - 2 * _g * mu, 1.5));
    for (i = 0; i < numSamples; ++i)
    {
        Vector3 samplePosition = r(tCurrent + T(0.5) * segmentLength);
        double height = samplePosition.magnitude() - radiusEarth;
        double hr = exp(-height / _Hr) * segmentLength;
        double hm = exp(-height / _Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        Ray lightRay(samplePosition, sunDirection);
        intersect(lightRay, radiusAtmosphere, lightRay.tmin, lightRay.tmax);
        double segmentLengthLight = lightRay.tmax / numSamplesLight;
        double tCurrentLight = 0.0;
        double opticalDepthLightR = 0.0;
        double opticalDepthLightM = 0.0;
        for (j = 0; j < numSamplesLight; ++j) {
            Vector3 samplePositionLight = lightRay(tCurrentLight + T(0.5) * segmentLengthLight);
            T heightLight = samplePositionLight.magnitude() - radiusEarth;
            if (heightLight < 0) break;
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            Vec3<T> tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1 * (opticalDepthM + opticalDepthLightM);
            Vec3<T> attenuation(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR +=  hr * attenuation;
            sumM +=  hm * attenuation;
        }
        tCurrent += segmentLength;
    }
    return 20 * (sumR * phaseR * _betaR + sumM * phaseM * _betaM);
}*/

Color rayleighGetSkyColor(Vector3 viewer, Vector3 direction, Vector3 sun_direction)
{
    return COLOR_BLACK;
}

Color rayleighApplyToObject(Vector3 viewer, Vector3 object_location, Vector3 sun_direction, Color object_color)
{
    return COLOR_BLACK;
}
