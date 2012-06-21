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

static inline Color _toColor(float x, float y, float Y)
{
	float fX, fY, fZ;
        Color result;

	fY = Y;
	fX = x / y * Y;
	fZ = ((1.0f - x - y) / y) * Y;

	float r, g, b;
		
	r =  3.2404f * fX - 1.5371f * fY - 0.4985f * fZ;
	g = -0.9692f * fX + 1.8759f * fY + 0.0415f * fZ;
	b =  0.0556f * fX - 0.2040f * fY + 1.0573f * fZ;

	float expo = -(1.0f / 10000.0f);
	r = 1.0f - exp(expo * r);
	g = 1.0f - exp(expo * g);
	b = 1.0f - exp(expo * b);

	if (r < 0.0f) r = 0.0f;
	if (g < 0.0f) g = 0.0f;
	if (b < 0.0f) b = 0.0f;

	result.r = r;
	result.g = g;
	result.b = b; 
        result.a = 1.0;
        
        colorNormalize(&result);
        
        return result;
}

static Color _preethamApproximate(SkyDefinition* definition, double theta, double phi)
{
    double thetaSun;
    double phiSun;
    double gamma;
    double turbidity = 2.0;

    /* Handle angles */
    if (theta > M_PI / 2.0)
    {
        theta = M_PI / 2.0;
    }
    if (definition->daytime <= 0.5)
    {
        thetaSun = M_PI - definition->daytime * 2.0 * M_PI;
        phiSun = 0.0;
    }
    else
    {
        thetaSun = (definition->daytime - 0.5) * 2.0 * M_PI;
        phiSun = M_PI;
    }
    gamma = _angleBetween(theta, phi, thetaSun, phiSun);
    
    double cosTheta;
    if (theta > M_PI / 2.0)
    {
        cosTheta = 0.0000001;
    }
    else
    {
        cosTheta = cos(theta);
    }
    
    double T = turbidity;
    double T2 = T * T;
    double suntheta = thetaSun;
    double suntheta2 = thetaSun * thetaSun;
    double suntheta3 = thetaSun * suntheta2;

    double Ax = -0.01925 * T - 0.25922;
    double Bx = -0.06651 * T + 0.00081; 
    double Cx = -0.00041 * T + 0.21247;
    double Dx = -0.06409 * T - 0.89887;
    double Ex = -0.00325 * T + 0.04517;

    double Ay = -0.01669 * T - 0.26078;
    double By = -0.09495 * T + 0.00921;
    double Cy = -0.00792 * T + 0.21023;
    double Dy = -0.04405 * T - 1.65369;
    double Ey = -0.01092 * T + 0.05291;

    double AY =  0.17872 * T - 1.46303;
    double BY = -0.35540 * T + 0.42749;
    double CY = -0.02266 * T + 5.32505;
    double DY =  0.12064 * T - 2.57705;
    double EY = -0.06696 * T + 0.37027;

    double cosGamma = cos(gamma);
    cosGamma = cosGamma < 0.0 ? 0.0 : cosGamma;
    double cosSTheta = fabs(cos(thetaSun));
    
    double xz = ( 0.00165 * suntheta3 - 0.00375 * suntheta2 + 0.00209 * suntheta + 0.00000) * T2 +
		 (-0.02903 * suntheta3 + 0.06377 * suntheta2 - 0.03202 * suntheta + 0.00394) * T +
		 ( 0.11693 * suntheta3 - 0.21196 * suntheta2 + 0.06052 * suntheta + 0.25886);

    double yz = ( 0.00275 * suntheta3 - 0.00610 * suntheta2 + 0.00317 * suntheta + 0.00000) * T2 +
                (-0.04214 * suntheta3 + 0.08970 * suntheta2 - 0.04153 * suntheta + 0.00516) * T +	
                ( 0.15346 * suntheta3 - 0.26756 * suntheta2 + 0.06670 * suntheta + 0.26688);
    
    double X = (4.0f / 9.0f - T / 120.0f) * (M_PI - 2.0 * suntheta);		
    double Yz = ((4.0453f * T - 4.9710) * tan(X) - 0.2155 * T + 2.4192) * 1000.0f;

    double val1, val2;
    
    val1 = ( 1 + Ax * exp(Bx / cosTheta ) ) * ( 1 + Cx * exp(Dx * gamma) + Ex * sqrt(cosGamma) );
    val2 = ( 1 + Ax * exp(Bx) ) * ( 1 + Cx * exp(Dx * suntheta) + Ex * sqrt(cosSTheta) );
    double x = xz * val1 / val2;
    
    val1 = ( 1 + Ay * exp(By / cosTheta) ) * ( 1 + Cy * exp(Dy * gamma   ) + Ey * sqrt(cosGamma )  );
    val2 = ( 1 + Ay * exp(By            )  ) * ( 1 + Cy * exp(Dy * suntheta) + Ey * sqrt(cosSTheta) );
    double y = yz * val1 / val2;
    
    val1 = ( 1 + AY * exp(BY / cosTheta) ) * ( 1 + CY * exp(DY * gamma   ) + EY * sqrt(cosGamma) );
    val2 = ( 1 + AY * exp(BY             ) ) * ( 1 + CY * exp(DY * suntheta) + EY * sqrt(cosSTheta) );
    double Y = Yz * val1 / val2;

    return _toColor(x, y, Y);
}

Color skyGetColor(SkyDefinition* definition, Renderer* renderer, Vector3 eye, Vector3 look)
{
    double dist;
    Vector3 sun_position;
    Color sun_color, sky_color;
    
    sun_position = skyGetSunDirection(definition);

    look = v3Normalize(look);
    dist = v3Norm(v3Sub(look, sun_position));

    sky_color = _preethamApproximate(definition, M_PI/2.0 - asin(look.y), atan2(-look.z, look.x));
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
