#include "LightStatus.h"

#include "LightingManager.h"
#include "Color.h"
#include "SurfaceMaterial.h"

LightStatus::LightStatus(LightingManager *manager, const Vector3 &location, const Vector3 &eye)
{
    this->manager = manager;
    this->location = location;
    this->eye = eye;
}

void LightStatus::pushComponent(LightComponent component)
{
    if (manager->alterLight(component, location))
    {
        components.push_back(component);
    }
}

Color LightStatus::apply(const Vector3 &normal, const SurfaceMaterial &material)
{
    Color final(0.0, 0.0, 0.0, 0.0);

    for (auto component: components)
    {
        final = final.add(manager->applyFinalComponent(component, eye, location, normal, material));
    }

    final.a = material.base.a;
    return final;
}
