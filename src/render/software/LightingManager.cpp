#include "LightingManager.h"

#include "LightFilter.h"
#include "LightComponent.h"
#include "LightStatus.h"
#include "LightSource.h"
#include "Color.h"
#include "SurfaceMaterial.h"

LightingManager::LightingManager()
{
    specularity = true;
}

int LightingManager::getStaticLightsCount() const
{
    return static_lights.size();
}

int LightingManager::getSourcesCount() const
{
    return sources.size();
}

int LightingManager::getFiltersCount() const
{
    return filters.size();
}

void LightingManager::clearStaticLights()
{
    static_lights.clear();
}

void LightingManager::addStaticLight(const LightComponent &light)
{
    static_lights.push_back(light);
}

void LightingManager::clearSources()
{
    sources.clear();
}

void LightingManager::registerSource(LightSource *source)
{
    if (std::find(sources.begin(), sources.end(), source) == sources.end())
    {
        sources.push_back(source);
    }
}

void LightingManager::unregisterSource(LightSource *source)
{
    if (std::find(sources.begin(), sources.end(), source) != sources.end())
    {
        sources.erase(std::find(sources.begin(), sources.end(), source));
    }
}

void LightingManager::clearFilters()
{
    filters.clear();
}

void LightingManager::registerFilter(LightFilter* filter)
{
    if (std::find(filters.begin(), filters.end(), filter) == filters.end())
    {
        filters.push_back(filter);
    }
}

void LightingManager::unregisterFilter(LightFilter *filter)
{
    if (std::find(filters.begin(), filters.end(), filter) != filters.end())
    {
        filters.erase(std::find(filters.begin(), filters.end(), filter));
    }
}

bool LightingManager::alterLight(LightComponent &component, const Vector3 &location)
{
    if (component.altered)
    {
        for (auto filter:filters)
        {
            if (not filter->applyLightFilter(component, location))
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

Color LightingManager::applyFinalComponent(const LightComponent &component, const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
{
    Color result, light_color;
    double normal_norm;
    Vector3 direction_inv;

    light_color = component.color;
    direction_inv = component.direction.normalize().scale(-1.0);

    normal_norm = normal.getNorm();
    if (normal_norm > 1.0)
    {
        normal_norm = 1.0;
    }

    result = COLOR_BLACK;

    /* diffused light */
    double diffuse = direction_inv.dotProduct(normal.normalize());
    double sign = (diffuse < 0.0) ? -1.0 : 1.0;
    if (material.hardness <= 0.5)
    {
        double hardness = material.hardness * 2.0;
        diffuse = (1.0 - hardness) * (diffuse * diffuse) * sign + hardness * diffuse;
    }
    else if (diffuse != 0.0)
    {
        double hardness = (material.hardness - 0.5) * 2.0;
        diffuse = (1.0 - hardness) * diffuse + hardness * sign * sqrt(fabs(diffuse));
    }
    diffuse = (diffuse + (1.0 - normal_norm)) / (1.0 + (1.0 - normal_norm));
    if (diffuse > 0.0)
    {
        result.r += diffuse * material.base->r * light_color.r;
        result.g += diffuse * material.base->g * light_color.g;
        result.b += diffuse * material.base->b * light_color.b;
    }

    /* specular reflection */
    if (material.shininess > 0.0 && material.reflection > 0.0 && component.reflection > 0.0)
    {
        Vector3 view = location.sub(eye).normalize();
        Vector3 reflect = direction_inv.sub(normal.scale(2.0 * direction_inv.dotProduct(normal)));
        double specular = reflect.dotProduct(view);
        if (specular > 0.0)
        {
            specular = pow(specular, material.shininess) * material.reflection * component.reflection * normal_norm;
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

void LightingManager::fillStatus(LightStatus &status, const Vector3 &location) const
{
    for (auto &light: static_lights)
    {
        status.pushComponent(light);
    }
    for (auto source: sources)
    {
        std::vector<LightComponent> lights;
        if (source->getLightsAt(lights, location))
        {
            for (auto &light: lights)
            {
                status.pushComponent(light);
            }
        }
    }
}

Color LightingManager::apply(const Vector3 &eye, const Vector3 &location, const Vector3 &normal, const SurfaceMaterial &material)
{
    LightStatus status(this, location, eye);
    fillStatus(status, location);
    return status.apply(normal, material);
}
