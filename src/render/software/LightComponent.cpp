#include "LightComponent.h"

LightComponent::LightComponent(const Color &color, const Vector3 &direction, double reflection, bool altered):
    color(color), direction(direction), reflection(reflection), altered(altered)
{
}
