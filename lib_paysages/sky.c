#include "sky.h"

#include <stdlib.h>
#include <math.h>

#include "shared/types.h"
#include "color.h"
#include "clouds.h"
#include "euclid.h"
#include "lighting.h"
#include "render.h"
#include "tools.h"

#define SPHERE_SIZE 1000.0

void skyInit()
{
}

void skyQuit()
{
}

void skySave(PackStream* stream, SkyDefinition* definition)
{
    packWriteDouble(stream, &definition->daytime);
    colorGradationSave(stream, definition->sun_color);
    packWriteDouble(stream, &definition->sun_radius);
    packWriteDouble(stream, &definition->sun_halo_size);
    curveSave(stream, definition->sun_halo_profile);
    colorGradationSave(stream, definition->zenith_color);
    colorGradationSave(stream, definition->haze_color);
    packWriteDouble(stream, &definition->haze_height);
    packWriteDouble(stream, &definition->haze_smoothing);
}

void skyLoad(PackStream* stream, SkyDefinition* definition)
{
    packReadDouble(stream, &definition->daytime);
    colorGradationLoad(stream, definition->sun_color);
    packReadDouble(stream, &definition->sun_radius);
    packReadDouble(stream, &definition->sun_halo_size);
    curveLoad(stream, definition->sun_halo_profile);
    colorGradationLoad(stream, definition->zenith_color);
    colorGradationLoad(stream, definition->haze_color);
    packReadDouble(stream, &definition->haze_height);
    packReadDouble(stream, &definition->haze_smoothing);

    skyValidateDefinition(definition);
}

SkyDefinition skyCreateDefinition()
{
    SkyDefinition def;

    def.daytime = 0.0;
    def.sun_color = colorGradationCreate();
    def.sun_radius = 1.0;
    def.sun_halo_size = 0.0;
    def.sun_halo_profile = curveCreate();
    def.zenith_color = colorGradationCreate();
    def.haze_color = colorGradationCreate();
    def.haze_height = 0.0;
    def.haze_smoothing = 0.0;

    skyValidateDefinition(&def);

    return def;
}

void skyDeleteDefinition(SkyDefinition* definition)
{
    curveDelete(definition->sun_halo_profile);
    colorGradationDelete(definition->sun_color);
    colorGradationDelete(definition->zenith_color);
    colorGradationDelete(definition->haze_color);
}

void skyCopyDefinition(SkyDefinition* source, SkyDefinition* destination)
{
    destination->daytime = source->daytime;
    destination->sun_radius = source->sun_radius;
    destination->sun_halo_size = source->sun_halo_size;
    destination->haze_height = source->haze_height;
    destination->haze_smoothing = source->haze_smoothing;
    
    curveCopy(source->sun_halo_profile, destination->sun_halo_profile);
    
    colorGradationCopy(source->sun_color, destination->sun_color);
    colorGradationCopy(source->zenith_color, destination->zenith_color);
    colorGradationCopy(source->haze_color, destination->haze_color);
    colorGradationCopy(source->_sky_gradation, destination->_sky_gradation);
}

void skyValidateDefinition(SkyDefinition* definition)
{
    Color zenith, haze;

    zenith = colorGradationGet(definition->zenith_color, definition->daytime);
    haze = colorGradationGet(definition->haze_color, definition->daytime);

    definition->_sky_gradation = colorGradationCreate();
    colorGradationQuickAdd(definition->_sky_gradation, 0.0, &haze);
    colorGradationQuickAdd(definition->_sky_gradation, definition->haze_height - definition->haze_smoothing, &haze);
    colorGradationQuickAdd(definition->_sky_gradation, definition->haze_height, &zenith);
    colorGradationQuickAdd(definition->_sky_gradation, 1.0, &zenith);
}

int skyGetLights(SkyDefinition* sky, LightDefinition* lights, int max_lights)
{
    double sun_angle;
    Vector3 sun_direction;
    int nblights = 0;

    sun_angle = (sky->daytime + 0.75) * M_PI * 2.0;
    sun_direction.x = cos(sun_angle);
    sun_direction.y = sin(sun_angle);
    sun_direction.z = 0.0;

    /* TODO Night lights */

    if (max_lights > 0)
    {
        /* Light from the sun */
        lights[0].direction = v3Scale(sun_direction, -1.0);
        lights[0].color = colorGradationGet(sky->sun_color, sky->daytime);
        lights[0].reflection = 1.0;
        lights[0].filtered = 1;
        lights[0].masked = 1;
        lights[0].amplitude = 0.0;
        nblights = 1;
        if (max_lights > 1)
        {
            /* Skydome lighting */
            lights[1].direction.x = 0.0;
            lights[1].direction.y = -1.0;
            lights[1].direction.z = 0.0;
            lights[1].color = colorGradationGet(sky->zenith_color, sky->daytime);
            lights[1].color.r *= 0.6;
            lights[1].color.g *= 0.6;
            lights[1].color.b *= 0.6;
            lights[1].reflection = 0.0;
            lights[1].filtered = 1;
            lights[1].masked = 0;
            lights[1].amplitude = M_PI / 2.0;
            nblights = 2;
        }
    }

    return nblights;
}

static inline double _angleBetween(double thetav, double phiv, double theta, double phi) 
{
    double cospsi = sin(thetav) * sin(theta) * cos(phi-phiv) + cos(thetav) * cos(theta);
    if (cospsi > 1.0) return 0.0;
    if (cospsi < -1.0) return M_PI;
    return acos(cospsi);
}

static inline double _perezFunction(double A, double B, double C, double D, double E, double Theta, double Gamma)
{
    double cosGamma = cos(Gamma);
    return (1.0 + A * exp(B / cos(Theta))) * (1.0 + C * exp(D * Gamma) + E * cosGamma * cosGamma);
}

typedef struct
{
    double x;
    double y;
    double Y;
} ColorxyY;

/* Distribution coefficients for the luminance(Y) distribution function */
static double YDC[5][2] = { { 0.1787, - 1.4630},
                            {-0.3554,   0.4275},
                            {-0.0227,   5.3251},
                            { 0.1206, - 2.5771},
                            {-0.0670,   0.3703} };

/* Distribution coefficients for the x distribution function */
static double xDC[5][2] = { {-0.0193, -0.2592},
                            {-0.0665, 0.0008},
                            {-0.0004, 0.2125},
                            {-0.0641, -0.8989},
                            {-0.0033, 0.0452} };

/* Distribution coefficients for the y distribution function */
static double yDC[5][2] = { {-0.0167, -0.2608},
                            {-0.0950, 0.0092},
                            {-0.0079, 0.2102},
                            {-0.0441, -1.6537},
                            {-0.0109, 0.0529} };

/* Zenith x value */
static double xZC[3][4] = {  {0.00166, -0.00375, 0.00209, 0},
                            {-0.02903, 0.06377, -0.03203, 0.00394},
                            {0.11693, -0.21196, 0.06052, 0.25886} };
/* Zenith y value */
static double yZC[3][4] = { { 0.00275, -0.00610, 0.00317, 0},
                            {-0.04214, 0.08970, -0.04153, 0.00516},
                            {0.15346, -0.26756, 0.06670, 0.26688} };

static double _distribution(double A, double B, double C, double D, double E, double Theta, double Gamma, double ThetaSun)
{
    double f0 = _perezFunction(A,B,C,D,E,Theta,Gamma);
    double f1 = _perezFunction(A,B,C,D,E,0,ThetaSun);
    return(f0/f1);
}

static double _chromaticity( double ZC[3][4], double ThetaSun, double turbidity)
{
    double t1 = ThetaSun;
    double t2 = t1*t1;
    double t3 = t2 * t1;

    double c = (ZC[0][0]*t3 + ZC[0][1]*t2 + ZC[0][2]*t1 + ZC[0][3])* turbidity * turbidity +
            (ZC[1][0]*t3 + ZC[1][1]*t2 + ZC[1][2]*t1 + ZC[1][3])* turbidity +
            (ZC[2][0]*t3 + ZC[2][1]*t2 + ZC[2][2]*t1 + ZC[2][3]);
    return c;
}

static Color _toColor(ColorxyY src)
{
    double X, Y, Z;
    Color dest;
    
    /* Convert to XYZ space */
    X = src.x * (src.Y / src.y);
    Y = src.Y;
    Z = (1.0 - src.x - src.y)* (src.Y/src.y);
    
    /* Convert to RGB */
    dest.r  =   3.240479 * X    - 1.537150 * Y  - 0.498535 * Z;
    dest.g = -  0.969256 * X    + 1.875991 * Y  + 0.041556 * Z;
    dest.b =    0.055648 * X    - 0.204043 * Y  + 1.057311 * Z;
    dest.a = 1.0;
    
    return dest;
}

static Color _preethamApproximate(SkyDefinition* definition, double theta, double phi)
{
    double thetaSun;
    double phiSun;
    double gamma;
    double turbidity = 3.0;
    ColorxyY skycolor;
    ColorxyY Zenith;
    double A,B,C,D,E;
    double d,chi;

    /* Handle angles */
    if (definition->daytime <= 0.5)
    {
        if (definition->daytime <= 0.25)
        {
            thetaSun = M_PI / 2.0 - 0.00001;
        }
        else
        {
            thetaSun = M_PI - definition->daytime * 2.0 * M_PI;
        }
        phiSun = 0.0;
    }
    else
    {
        if (definition->daytime >= 0.75)
        {
            thetaSun = M_PI / 2.0 - 0.00001;
        }
        else
        {
            thetaSun = (definition->daytime - 0.5) * 2.0 * M_PI;
        }
        phiSun = M_PI;
    }
    gamma = _angleBetween(theta, phi, thetaSun, phiSun);

    /* Zenith luminance */
    chi = (4.0/9.0 - turbidity/120.0)*(M_PI - 2*thetaSun);
    Zenith.Y = (4.0453*turbidity - 4.9710)*tan(chi) - 0.2155*turbidity + 2.4192;
    if (Zenith.Y < 0.0) Zenith.Y = -Zenith.Y;

    A = YDC[0][0]*turbidity + YDC[0][1];
    B = YDC[1][0]*turbidity + YDC[1][1];
    C = YDC[2][0]*turbidity + YDC[2][1];
    D = YDC[3][0]*turbidity + YDC[3][1];
    E = YDC[4][0]*turbidity + YDC[4][1];

    /* Sky luminance */
    d = _distribution(A, B, C, D, E, theta, gamma, thetaSun);
    skycolor.Y = Zenith.Y * d;

    /* Zenith x */
    Zenith.x = _chromaticity( xZC, thetaSun, turbidity );
    A = xDC[0][0]*turbidity + xDC[0][1];
    B = xDC[1][0]*turbidity + xDC[1][1];
    C = xDC[2][0]*turbidity + xDC[2][1];
    D = xDC[3][0]*turbidity + xDC[3][1];
    E = xDC[4][0]*turbidity + xDC[4][1];

    /* Sky x */
    d = _distribution(A, B, C, D, E, theta, gamma, thetaSun);
    skycolor.x = Zenith.x * d;

    /* Zenith y */
    Zenith.y = _chromaticity( yZC, thetaSun, turbidity );
    A = yDC[0][0]*turbidity + yDC[0][1];
    B = yDC[1][0]*turbidity + yDC[1][1];
    C = yDC[2][0]*turbidity + yDC[2][1];
    D = yDC[3][0]*turbidity + yDC[3][1];
    E = yDC[4][0]*turbidity + yDC[4][1];

    /* Sky y */
    d = _distribution(A, B, C, D, E, theta, gamma, thetaSun);
    skycolor.y = Zenith.y * d;

    /* Small hack on luminance value */
    //skycolor.Y = 1 - exp(-(1.0/25.0) * skycolor.Y);

    return _toColor(skycolor);
}

Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look)
{
    double dist;
    Vector3 sun_position;
    Color sun_color, sky_color;
    
    sun_position = skyGetSunDirection(definition);

    look = v3Normalize(look);
    dist = v3Norm(v3Sub(look, sun_position));

    sky_color = _preethamApproximate(definition, M_PI/2.0 - asin(look.y), acos(v3Dot(look, sun_position)));
    if (dist < definition->sun_radius + definition->sun_halo_size)
    {
        sun_color = colorGradationGet(definition->sun_color, definition->daytime);
        if (dist <= definition->sun_radius)
        {
            return sun_color;
        }
        else
        {
            dist = (dist - definition->sun_radius) / definition->sun_halo_size;
            sun_color.a = curveGetValue(definition->sun_halo_profile, dist);
            colorMask(&sky_color, &sun_color);
            return sky_color;
        }
    }
    else
    {
        return sky_color;
    }
}

static Color _postProcessFragment(Renderer* renderer, Vector3 location, void* data)
{
    Vector3 direction;
    Color result;
    SkyDefinition* definition;

    definition = (SkyDefinition*)data;

    direction = v3Sub(location, renderer->camera_location);

    result = skyGetColor(definition, renderer, renderer->camera_location, v3Normalize(direction));
    result = renderer->applyClouds(renderer, result, renderer->camera_location, v3Add(renderer->camera_location, v3Scale(direction, 10.0)));

    return result;
}

void skyRender(SkyDefinition* definition, Renderer* renderer)
{
    int res_i, res_j;
    int i, j;
    double step_i, step_j;
    double current_i, current_j;
    Vector3 vertex1, vertex2, vertex3, vertex4;
    Vector3 direction;

    res_i = renderer->render_quality * 40;
    res_j = renderer->render_quality * 20;
    step_i = M_PI * 2.0 / (double)res_i;
    step_j = M_PI / (double)res_j;

    for (j = 0; j < res_j; j++)
    {
        if (!renderer->addRenderProgress(renderer, 0.0))
        {
            return;
        }

        current_j = (double)(j - res_j / 2) * step_j;

        for (i = 0; i < res_i; i++)
        {
            current_i = (double)i * step_i;

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j);
            vertex1 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j);
            direction.y = SPHERE_SIZE * sin(current_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j);
            vertex2 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i + step_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i + step_i) * cos(current_j + step_j);
            vertex3 = v3Add(renderer->camera_location, direction);

            direction.x = SPHERE_SIZE * cos(current_i) * cos(current_j + step_j);
            direction.y = SPHERE_SIZE * sin(current_j + step_j);
            direction.z = SPHERE_SIZE * sin(current_i) * cos(current_j + step_j);
            vertex4 = v3Add(renderer->camera_location, direction);

            /* TODO Triangles at poles */
            renderer->pushQuad(renderer, vertex1, vertex4, vertex3, vertex2, _postProcessFragment, definition);
        }
    }
}

Vector3 skyGetSunDirection(SkyDefinition* definition)
{
    Vector3 result;
    double sun_angle = (definition->daytime + 0.75) * M_PI * 2.0;
    result.x = cos(sun_angle);
    result.y = sin(sun_angle);
    result.z = 0.0;
    return result;
}

Color skyGetSunColor(SkyDefinition* definition)
{
    return colorGradationGet(definition->sun_color, definition->daytime);
}

Color skyGetZenithColor(SkyDefinition* definition)
{
    return colorGradationGet(definition->zenith_color, definition->daytime);
}
