#include "LightingManager.h"

#include "LightFilter.h"
#include "LightComponent.h"
#include "Color.h"

LightingManager::LightingManager()
{
    specularity = true;
}

void LightingManager::registerFilter(LightFilter* filter)
{
    if (not filters.contains(filter))
    {
        filters.add(filter);
    }
}

bool LightingManager::alterLight(LightComponent &component, const Vector3 &location)
{
    if (component.altered)
    {
        for (auto filter:filters)
        {
            if (not filter.app(component, location))
            {
                return false;
            }
        }
    }

    if (not specularity)
    {
        component.reflection = 0.0;
    }

    return component.color.getPower() > 0.0001;
}

void LightingManager::setSpecularity(bool enabled)
{
    specularity = enabled;
}

void applyFinalComponent(const LightComponent &component, const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
{
    Color result, light_color;
    double normal_norm;
    Vector3 direction_inv;

    light_color = component->color;
    direction_inv = v3Scale(v3Normalize(component->direction), -1.0);

    normal_norm = v3Norm(normal);
    if (normal_norm > 1.0)
    {
        normal_norm = 1.0;
    }
    normal = v3Normalize(normal);

    result = COLOR_BLACK;

    /* diffused light */
    double diffuse = v3Dot(direction_inv, normal);
    diffuse = (diffuse + (1.0 - normal_norm)) / (1.0 + (1.0 - normal_norm));
    if (diffuse > 0.0)
    {
        result.r += diffuse * material->_rgb.r * light_color.r;
        result.g += diffuse * material->_rgb.g * light_color.g;
        result.b += diffuse * material->_rgb.b * light_color.b;
    }

    /* specular reflection */
    if (material->reflection > 0.0 && light->reflection > 0.0)
    {
        Vector3 view = v3Normalize(v3Sub(location, eye));
        Vector3 reflect = v3Sub(direction_inv, v3Scale(normal, 2.0 * v3Dot(direction_inv, normal)));
        double specular = v3Dot(reflect, view);
        if (specular > 0.0)
        {
            specular = pow(specular, material->shininess) * material->reflection * light->reflection * normal_norm;
            if (specular > 0.0)
            {
                result.r += specular * light_color.r;
                result.g += specular * light_color.g;
                result.b += specular * light_color.b;
            }
        }
    }

    /* specular reflection with fresnel effect */
    /*if (material->reflection > 0.0 && light->reflection > 0.0)
    {
        Vector3 view = v3Normalize(v3Sub(location, eye));
        Vector3 h = v3Normalize(v3Sub(direction_inv, view));
        double fresnel = 0.02 + 0.98 * pow(1.0 - v3Dot(v3Scale(view, -1.0), h), 5.0);
        double refl = v3Dot(h, normal);
        if (refl > 0.0)
        {
            double waterBrdf = fresnel * pow(refl, material->shininess);
            if (waterBrdf > 0.0)
            {
                refl = material->reflection * waterBrdf * light->reflection;
                result.r += refl * light_color.r;
                result.g += refl * light_color.g;
                result.b += refl * light_color.b;
            }
        }
    }*/

    return result;
}
