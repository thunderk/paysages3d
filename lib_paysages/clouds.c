#include "clouds.h"

#include <string.h>
#include <math.h>

#include "lighting.h"
#include "shared/types.h"
#include "shared/functions.h"
#include "shared/constants.h"
#include "shared/globals.h"

typedef struct
{
    Vector3 start;
    Vector3 end;
    double length;
} CloudSegment;

static CloudsLayerDefinition NULL_LAYER;

void cloudsInit()
{
    NULL_LAYER.noise = noiseCreateGenerator();
    NULL_LAYER.coverage = 0.0;
}

void cloudsSave(FILE* f, CloudsDefinition* definition)
{
    int i;
    CloudsLayerDefinition* layer;

    toolsSaveInt(f, definition->nblayers);
    for (i = 0; i < definition->nblayers; i++)
    {
        layer = definition->layers + i;
        toolsSaveDouble(f, layer->ycenter);
        toolsSaveDouble(f, layer->ymin);
        toolsSaveDouble(f, layer->ymax);
        noiseSave(layer->noise, f);
        colorSave(layer->color, f);
        toolsSaveDouble(f, layer->scaling);
        toolsSaveDouble(f, layer->coverage);
    }
}

void cloudsLoad(FILE* f, CloudsDefinition* definition)
{
    int i, n;
    CloudsLayerDefinition* layer;

    while (definition->nblayers > 0)
    {
        cloudsDeleteLayer(definition, 0);
    }

    n = toolsLoadInt(f);
    for (i = 0; i < n; i++)
    {
        layer = definition->layers + cloudsAddLayer(definition);

        layer->ycenter = toolsLoadDouble(f);
        layer->ymin = toolsLoadDouble(f);
        layer->ymax = toolsLoadDouble(f);
        noiseLoad(layer->noise, f);
        layer->color = colorLoad(f);
        layer->scaling = toolsLoadDouble(f);
        layer->coverage = toolsLoadDouble(f);
    }
}

CloudsDefinition cloudsCreateDefinition()
{
    CloudsDefinition result;

    result.nblayers = 0;

    return result;
}

void cloudsDeleteDefinition(CloudsDefinition* definition)
{
    while (definition->nblayers > 0)
    {
        cloudsDeleteLayer(definition, 0);
    }
}

void cloudsCopyDefinition(CloudsDefinition* source, CloudsDefinition* destination)
{
    CloudsLayerDefinition* layer;
    int i;

    while (destination->nblayers > 0)
    {
        cloudsDeleteLayer(destination, 0);
    }
    for (i = 0; i < source->nblayers; i++)
    {
        layer = cloudsGetLayer(destination, cloudsAddLayer(destination));
        cloudsLayerCopyDefinition(source->layers + i, layer);
    }
}

void cloudsValidateDefinition(CloudsDefinition* definition)
{
    int i;
    for (i = 0; i < definition->nblayers; i++)
    {
        cloudsLayerValidateDefinition(&definition->layers[i]);
    }
}

CloudsLayerDefinition cloudsLayerCreateDefinition()
{
    CloudsLayerDefinition result;

    result.color = COLOR_BLACK;
    result.coverage = 0.0;
    result.noise = noiseCreateGenerator();
    result.scaling = 1.0;
    result.ymin = 0.0;
    result.ycenter = 0.5;
    result.ymax = 1.0;

    return result;
}

void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition)
{
    noiseDeleteGenerator(definition->noise);
}

void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination)
{
    NoiseGenerator* noise;

    noise = destination->noise;
    *destination = *source;
    destination->noise = noise;
    noiseCopy(source->noise, destination->noise);
}

void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition)
{
    if (definition->coverage < 0.5)
    {
        noiseNormalizeHeight(definition->noise, -1.0, definition->coverage * 2.0, 0);
    }
    else
    {
        noiseNormalizeHeight(definition->noise, -(1.0 - definition->coverage) * 2.0, 1.0, 0);
    }
}

int cloudsGetLayerCount(CloudsDefinition* definition)
{
    return definition->nblayers;
}

CloudsLayerDefinition* cloudsGetLayer(CloudsDefinition* definition, int layer)
{
    if (layer >= 0 && layer < definition->nblayers)
    {
        return definition->layers + layer;
    }
    else
    {
        return &NULL_LAYER;
    }
}

int cloudsAddLayer(CloudsDefinition* definition)
{
    CloudsLayerDefinition* layer;

    if (definition->nblayers < CLOUDS_MAX_LAYERS)
    {
        layer = definition->layers + definition->nblayers;
        layer->noise = noiseCreateGenerator();
        layer->coverage = 0.0;

        return definition->nblayers++;
    }
    else
    {
        return -1;
    }
}

void cloudsDeleteLayer(CloudsDefinition* definition, int layer)
{
    if (layer >= 0 && layer < definition->nblayers)
    {
        cloudsLayerDeleteDefinition(definition->layers + layer);
        if (definition->nblayers > 1 && layer < definition->nblayers - 1)
        {
            memmove(definition->layers + layer, definition->layers + layer + 1, sizeof(CloudsLayerDefinition) * (definition->nblayers - layer - 1));
        }
        definition->nblayers--;
    }
}

static inline double _getDistanceToBorder(CloudsLayerDefinition* layer, Vector3 position, double detail)
{
    double val, min;

    if (position.y > layer->ycenter)
    {
        min = (position.y - layer->ycenter) / (layer->ymax - layer->ycenter);
    }
    else
    {
        min = (layer->ycenter - position.y) / (layer->ycenter - layer->ymin);
    }

    val = noiseGet3DDetail(layer->noise, position.x, position.y, position.z, detail);

    return (val - min) * layer->scaling;
}

static inline Vector3 _getNormal(CloudsLayerDefinition* layer, Vector3 position, double detail)
{
    Vector3 result = {0.0, 0.0, 0.0};
    double val, dval;

    val = noiseGet3DDetail(layer->noise, position.x, position.y, position.z, detail);

    dval = val - noiseGet3DDetail(layer->noise, position.x + detail, position.y, position.z, detail);
    result.x += dval;

    dval = val - noiseGet3DDetail(layer->noise, position.x - detail, position.y, position.z, detail);
    result.x -= dval;

    dval = val - noiseGet3DDetail(layer->noise, position.x, position.y + detail, position.z, detail);
    result.y += dval;

    dval = val - noiseGet3DDetail(layer->noise, position.x, position.y - detail, position.z, detail);
    result.y -= dval;

    dval = val - noiseGet3DDetail(layer->noise, position.x, position.y, position.z + detail, detail);
    result.z += dval;

    dval = val - noiseGet3DDetail(layer->noise, position.x, position.y, position.z - detail, detail);
    result.z -= dval;

    return v3Normalize(result);
}

/**
 * Optimize the search limits in a layer.
 *
 * @param layer The cloud layer
 * @param start Start of the search to optimize
 * @param end End of the search to optimize
 * @return 0 if the search is useless
 */
static int _optimizeSearchLimits(CloudsLayerDefinition* layer, Vector3* start, Vector3* end)
{
    Vector3 diff;

    if (start->y > layer->ymax)
    {
        if (end->y >= layer->ymax)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->ymax - start->y) / diff.y));
            if (end->y < layer->ymin)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->ymin - end->y) / diff.y));
            }
        }
    }
    else if (start->y < layer->ymin)
    {
        if (end->y <= layer->ymin)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->ymin - start->y) / diff.y));
            if (end->y > layer->ymax)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->ymax - end->y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        diff = v3Sub(*end, *start);
        if (end->y > layer->ymax)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->ymax - start->y) / diff.y));
        }
        else if (end->y < layer->ymin)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->ymin - start->y) / diff.y));
        }
    }

    /* TODO Limit the search length */
    return 1;
}

/**
 * Go through the cloud layer to find segments (parts of the lookup that are inside the cloud).
 *
 * @param definition The cloud layer
 * @param renderer The renderer environment
 * @param start Start position of the lookup (already optimized)
 * @param direction Normalized direction of the lookup
 * @param detail Level of noise detail required
 * @param max_segments Maximum number of segments to collect
 * @param max_inside_length Maximum length to spend inside the cloud
 * @param max_total_length Maximum lookup length
 * @param inside_length Resulting length inside cloud (sum of all segments length)
 * @param total_length Resulting lookup length
 * @param out_segments Allocated space to fill found segments
 * @return Number of segments found
 */
static int _findSegments(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 start, Vector3 direction, double detail, int max_segments, double max_inside_length, double max_total_length, double* inside_length, double* total_length, CloudSegment* out_segments)
{
    int inside, segment_count;
    double current_total_length, current_inside_length;
    double step_length, segment_length, remaining_length;
    double noise_distance, last_noise_distance;
    Vector3 walker, step, segment_start;
    double render_precision;

    if (max_segments <= 0)
    {
        return 0;
    }

    render_precision = 3.3 - 0.3 * (double)renderer->render_quality;

    segment_count = 0;
    current_total_length = 0.0;
    current_inside_length = 0.0;
    segment_length = 0.0;
    walker = start;
    noise_distance = _getDistanceToBorder(definition, start, detail) * render_precision;
    inside = (noise_distance > 0.0) ? 1 : 0;
    step = v3Scale(direction, render_precision);

    do
    {
        walker = v3Add(walker, step);
        step_length = v3Norm(step);
        last_noise_distance = noise_distance;
        noise_distance = _getDistanceToBorder(definition, walker, detail) * render_precision;
        current_total_length += step_length;

        if (noise_distance > 0.0)
        {
            if (inside)
            {
                // inside the cloud
                segment_length += step_length;
                current_inside_length += step_length;
                step = v3Scale(direction, (noise_distance < render_precision) ? render_precision : noise_distance);
            }
            else
            {
                // entering the cloud
                inside = 1;
                segment_length = step_length * noise_distance / (noise_distance - last_noise_distance);
                segment_start = v3Add(walker, v3Scale(direction, -segment_length));
                current_inside_length += segment_length;
                step = v3Scale(direction, render_precision);
            }
        }
        else
        {
            if (inside)
            {
                // exiting the cloud
                remaining_length = step_length * last_noise_distance / (last_noise_distance - noise_distance);
                segment_length += remaining_length;
                current_inside_length += remaining_length;

                out_segments->start = segment_start;
                out_segments->end = v3Add(walker, v3Scale(direction, remaining_length - step_length));
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments)
                {
                    break;
                }

                inside = 0;
                step = v3Scale(direction, render_precision);
            }
            else
            {
                // searching for a cloud
                step = v3Scale(direction, (noise_distance > -render_precision) ? render_precision : -noise_distance);
            }
        }
    } while (inside || (walker.y <= definition->ymax + 0.001 && walker.y >= definition->ymin - 0.001 && current_total_length < max_total_length && current_inside_length < max_inside_length));

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
}

/*static Color _lightFilter(Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light, void* custom_data)
{
    double inside_depth, total_depth;
    CloudSegment segments[20];
    LightFilterData data;

    data = *((LightFilterData*)custom_data);
    data.detail = (data.detail < 0.1) ? 0.1 : data.detail;

    // FIXME Dont hard-code max_total_length
    _findSegments(data.definition, data.quality, location, direction_to_light, data.detail, 20, 50.0, 300.0, &inside_depth, &total_depth, segments);

    inside_depth *= 0.02;
    if (inside_depth > 1.0)
    {
        inside_depth = 1.0;
    }

    light.r = light.r * (1.0 - 0.2 * inside_depth);
    light.g = light.g * (1.0 - 0.2 * inside_depth);
    light.b = light.b * (1.0 - 0.2 * inside_depth);

    return light;
}*/

static Color _applyLayerLighting(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 position, Color base, double detail)
{
    return base;
    /*Vector3 normal;
    ReceiverMaterial material;
    LightingEnvironment lighting_environment;
    LightFilterData data;

    normal = v3Scale(_getNormal(definition, position, 1.0), 0.25);
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.5), 0.25));
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.2), 0.25));
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.1), 0.25));
    normal = v3Normalize(normal);

    data.definition = definition;
    data.quality = quality;
    data.detail = detail;

    lighting_environment.filter = _lightFilter;
    lighting_environment.custom_data = &data;

    material.base = base;
    material.reflection = 0.3;
    material.shininess = 0.1;

    return lightingApplyCustom(position, normal, material, NULL, NULL, &lighting_environment);*/
}

Color cloudsGetLayerColor(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 start, Vector3 end)
{
    int i, segment_count;
    double max_length, detail, total_length, inside_length;
    Vector3 direction;
    Color result, col;
    CloudSegment segments[20];

    if (!_optimizeSearchLimits(definition, &start, &end))
    {
        return COLOR_TRANSPARENT;
    }

    direction = v3Sub(end, start);
    max_length = v3Norm(direction);
    direction = v3Normalize(direction);
    result = COLOR_TRANSPARENT;

    detail = renderer->getPrecision(renderer, start) / definition->scaling;

    segment_count = _findSegments(definition, renderer, start, direction, detail, 20, 60.0, max_length, &inside_length, &total_length, segments);
    for (i = 0; i < segment_count; i++)
    {
        col = _applyLayerLighting(definition, renderer, segments[i].start, definition->color, detail);
        col.a = (segments[i].length >= 50.0) ? 1.0 : (segments[i].length / 50.0);
        colorMask(&result, &col);
    }
    return result;
}

Color cloudsGetColor(CloudsDefinition* definition, Renderer* renderer, Vector3 start, Vector3 end)
{
    int i;
    Color layer_color, result;

    if (end.y < start.y)
    {
        return cloudsGetColor(definition, renderer, end, start);
    }

    if (definition->nblayers < 1 || end.y - start.y < 0.001)
    {
        return COLOR_TRANSPARENT;
    }

    result = COLOR_TRANSPARENT;
    for (i = 0; i < definition->nblayers; i++)
    {
        layer_color = cloudsGetLayerColor(definition->layers + i, renderer, start, end);
        if (layer_color.a > 0.0)
        {
            colorMask(&result, &layer_color);
        }
    }

    return result;
}
