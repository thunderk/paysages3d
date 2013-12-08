#include "LightStatus.h"

LightStatus::LightStatus(LightingManager *manager, const Vector3 &location, const Vector3 &eye)
{
    this->manager = manager;
    this->location = location;
    this->eye = eye;
}

void LightStatus::pushComponent(LightComponent component)
{
    if (manager->filter(&component, location))
    {
        components.add(component);
    }
}

Color LightStatus::apply(const Vector3 &normal, const SurfaceMaterial &material)
{
    Color final();

    for (auto component: components)
    {
        final.add(manager->applyFinalComponent(component, eye, location, normal, material));
    }

    final.a = material->base.a;
    return final;
}
