#include "renderer.h"
#include "shared/constants.h"

static Color _fakeFilterLight(Renderer* renderer, Color light_color, Vector3 at_location, Vector3 light_location, Vector3 direction_to_light)
{
    return light_color;
}

Renderer rendererGetFake()
{
    Renderer result;

    result.render_quality = 5;
    result.camera_location = VECTOR_ZERO;

    result.filterLight = _fakeFilterLight;

    return result;
}
