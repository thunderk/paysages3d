#include "NightSky.h"

#include "Color.h"
#include "Vector3.h"
#include "Geometry.h"

NightSky::NightSky(SoftwareRenderer* renderer):
    renderer(renderer)
{
}

NightSky::~NightSky()
{
}

void NightSky::update()
{
}

const Color NightSky::getColor(double altitude, const Vector3 &direction)
{
    Color result(0.01, 0.012, 0.03);

    Vector3 location(0.0, altitude, 0.0);

    // Get stars

    // Get moon
    Vector3 moon_direction(0.3, 0.4, -0.3);
    moon_direction.normalize();
    if (moon_direction.dotProduct(direction) >= 0)
    {
        Vector3 moon_position = moon_direction.scale(100.0);
        double moon_radius = 1.0;
        Vector3 hit1, hit2;
        int hits = Geometry::rayIntersectSphere(location, direction, moon_position, moon_radius, &hit1, &hit2);
        if (hits > 1)
        {
            double dist = hit2.sub(hit1).getNorm() / moon_radius; // distance between intersection points (relative to radius)

            Color moon_color(0.3, 0.3, 0.3);
            if (dist <= 0.05)
            {
                moon_color.a *= 1.0 - dist / 0.05;
            }
            result.mask(moon_color);
        }
    }


    return result;

}
