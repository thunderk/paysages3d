#include "LightingManager.h"

#include "Color.h"
#include "LightComponent.h"
#include "LightFilter.h"
#include "LightSource.h"
#include "LightStatus.h"
#include "Logs.h"
#include "SurfaceMaterial.h"
#include <algorithm>
#include <cmath>

LightingManager::LightingManager() {
    specularity = true;
    filtering = true;
}

int LightingManager::getStaticLightsCount() const {
    return static_lights.size();
}

int LightingManager::getSourcesCount() const {
    return sources.size();
}

int LightingManager::getFiltersCount() const {
    return filters.size();
}

void LightingManager::clearStaticLights() {
    static_lights.clear();
}

void LightingManager::addStaticLight(const LightComponent &light) {
    static_lights.push_back(light);
}

void LightingManager::clearSources() {
    sources.clear();
}

void LightingManager::registerSource(LightSource *source) {
    if (find(sources.begin(), sources.end(), source) == sources.end()) {
        sources.push_back(source);
    }
}

void LightingManager::unregisterSource(LightSource *source) {
    if (find(sources.begin(), sources.end(), source) != sources.end()) {
        sources.erase(find(sources.begin(), sources.end(), source));
    }
}

void LightingManager::clearFilters() {
    filters.clear();
}

void LightingManager::registerFilter(LightFilter *filter) {
    if (find(filters.begin(), filters.end(), filter) == filters.end()) {
        filters.push_back(filter);
    }
}

void LightingManager::unregisterFilter(LightFilter *filter) {
    if (find(filters.begin(), filters.end(), filter) != filters.end()) {
        filters.erase(find(filters.begin(), filters.end(), filter));
    }
}

bool LightingManager::alterLight(LightComponent &component, const Vector3 &location) {
    if (filtering and component.altered) {
        for (auto filter : filters) {
            if (not filter->applyLightFilter(component, location)) {
                return false;
            }
        }
    }

    if (not specularity) {
        component.reflection = 0.0;
    }

    return component.color.getPower() > 0.0001;
}

void LightingManager::setSpecularity(bool enabled) {
    specularity = enabled;
}

void LightingManager::setFiltering(bool enabled) {
    filtering = enabled;
}

Color LightingManager::applyFinalComponent(const LightComponent &component, const Vector3 &eye, const Vector3 &location,
                                           const Vector3 &normal, const SurfaceMaterial &material) {
    Color result, light_color;
    Vector3 direction_inv;

    light_color = component.color;
    direction_inv = component.direction.normalize().scale(-1.0);
    normal.normalize();

    result = COLOR_BLACK;

    /* diffused light */
    double diffuse = direction_inv.dotProduct(normal);
    double sign = (diffuse < 0.0) ? -1.0 : 1.0;
    if (material.hardness <= 0.5) {
        double hardness = material.hardness * 2.0;
        diffuse = (1.0 - hardness) * (diffuse * diffuse) * sign + hardness * diffuse;
    } else if (diffuse != 0.0) {
        double hardness = (material.hardness - 0.5) * 2.0;
        diffuse = (1.0 - hardness) * diffuse + hardness * sign * sqrt(fabs(diffuse));
    }
    if (material.ambient > 0.0) {
        diffuse = material.ambient + (1.0 - material.ambient) * diffuse;
    }
    if (diffuse > 0.0) {
        result.r += diffuse * material.base->r * light_color.r;
        result.g += diffuse * material.base->g * light_color.g;
        result.b += diffuse * material.base->b * light_color.b;
    }

    /* specular reflection */
    if (specularity && sign > 0.0 && material.shininess > 0.0 && material.reflection > 0.0 &&
        component.reflection > 0.0) {
        Vector3 view = location.sub(eye).normalize();
        Vector3 reflect = direction_inv.sub(normal.scale(2.0 * direction_inv.dotProduct(normal)));
        double specular = reflect.dotProduct(view);
        if (specular > 0.0) {
            specular = pow(specular, material.shininess) * material.reflection * component.reflection;
            if (specular > 0.0) {
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

void LightingManager::fillStatus(LightStatus &status, const Vector3 &location) const {
    for (auto &light : static_lights) {
        status.pushComponent(light);
    }
    for (auto source : sources) {
        vector<LightComponent> lights;
        if (source->getLightsAt(lights, location)) {
            for (auto &light : lights) {
                status.pushComponent(light);
            }
        }
    }
}

Color LightingManager::apply(const Vector3 &eye, const Vector3 &location, const Vector3 &normal,
                             const SurfaceMaterial &material) {
    LightStatus status(this, location, eye);
    fillStatus(status, location);
    return status.apply(normal, material);
}
