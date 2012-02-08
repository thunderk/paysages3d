#include "clouds.h"

#include <string.h>
#include <math.h>
#include "color.h"
#include "euclid.h"
#include "lighting.h"
#include "tools.h"
#include "shared/types.h"
#include "shared/constants.h"

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

    toolsSaveInt(f, &definition->nblayers);
    for (i = 0; i < definition->nblayers; i++)
    {
        layer = definition->layers + i;
        
        toolsSaveDouble(f, &layer->ycenter);
        toolsSaveDouble(f, &layer->ymin);
        toolsSaveDouble(f, &layer->ymax);
        noiseSave(f, layer->noise);
        materialSave(f, &layer->material);
        toolsSaveDouble(f, &layer->transparencydepth);
        toolsSaveDouble(f, &layer->lighttraversal);
        toolsSaveDouble(f, &layer->minimumlight);
        toolsSaveDouble(f, &layer->scaling);
        toolsSaveDouble(f, &layer->coverage);
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

    toolsLoadInt(f, &n);
    for (i = 0; i < n; i++)
    {
        layer = definition->layers + cloudsAddLayer(definition);

        toolsLoadDouble(f, &layer->ycenter);
        toolsLoadDouble(f, &layer->ymin);
        toolsLoadDouble(f, &layer->ymax);
        noiseLoad(f, layer->noise);
        materialLoad(f, &layer->material);
        toolsLoadDouble(f, &layer->transparencydepth);
        toolsLoadDouble(f, &layer->lighttraversal);
        toolsLoadDouble(f, &layer->minimumlight);
        toolsLoadDouble(f, &layer->scaling);
        toolsLoadDouble(f, &layer->coverage);
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

static double _standardCoverageFunc(CloudsLayerDefinition* layer, Vector3 position)
{
    double inside;
    
    if (position.y > layer->ycenter)
    {
        inside = 1.0 - (position.y - layer->ycenter) / (layer->ymax - layer->ycenter);
    }
    else
    {
        inside = 1.0 - (layer->ycenter - position.y) / (layer->ycenter - layer->ymin);
    }
    
    if (inside <= 0.0)
    {
        return 0.0;
    }
    else
    {    
        return layer->coverage * inside;
    }
}

CloudsLayerDefinition cloudsLayerCreateDefinition()
{
    CloudsLayerDefinition result;

    result.material.base = COLOR_WHITE;
    result.material.reflection = 0.1;
    result.material.shininess = 2.0;
    result.transparencydepth = 50.0;
    result.lighttraversal = 50.0;
    result.minimumlight = 0.8;
    result.coverage = 0.5;
    result.noise = noiseCreateGenerator();
    result.scaling = 50.0;
    result.ymin = 50.0;
    result.ycenter = 100.0;
    result.ymax = 200.0;
    
    result.customcoverage = _standardCoverageFunc;

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
    /*if (definition->coverage < 0.5)
    {
        noiseNormalizeHeight(definition->noise, -1.0, definition->coverage * 2.0, 0);
    }
    else
    {
        noiseNormalizeHeight(definition->noise, -(1.0 - definition->coverage) * 2.0, 1.0, 0);
    }*/
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
        *layer = cloudsLayerCreateDefinition();

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

static inline double _getDistanceToBorder(CloudsLayerDefinition* layer, Vector3 position)
{
    double val;

    val = 0.5 * noiseGet3DTotal(layer->noise, position.x / layer->scaling, position.y / layer->scaling, position.z / layer->scaling) / noiseGetMaxValue(layer->noise);

    return (val - 0.5 + layer->customcoverage(layer, position)) * layer->scaling;
}

static inline Vector3 _getNormal(CloudsLayerDefinition* layer, Vector3 position, double detail)
{
    Vector3 result = {0.0, 0.0, 0.0};
    Vector3 dposition;
    double val, dval;
    
    val = _getDistanceToBorder(layer, position);

    dposition.x = position.x + detail;
    dposition.y = position.y;
    dposition.z = position.z;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x += dval;

    dposition.x = position.x - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.x -= dval;

    dposition.x = position.x;
    dposition.y = position.y + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y += dval;

    dposition.y = position.y - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.y -= dval;

    dposition.y = position.y;
    dposition.z = position.z + detail;
    dval = val - _getDistanceToBorder(layer, dposition);
    result.z += dval;

    dposition.z = position.z - detail;
    dval = val - _getDistanceToBorder(layer, dposition);
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
    noise_distance = _getDistanceToBorder(definition, start) * render_precision;
    inside = (noise_distance > 0.0) ? 1 : 0;
    step = v3Scale(direction, render_precision);

    do
    {
        walker = v3Add(walker, step);
        step_length = v3Norm(step);
        last_noise_distance = noise_distance;
        noise_distance = _getDistanceToBorder(definition, walker) * render_precision;
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

static Color _applyLayerLighting(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 position, double detail)
{
    Vector3 normal;

    normal = v3Scale(_getNormal(definition, position, 1.0), 0.25);
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.5), 0.25));
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.2), 0.25));
    normal = v3Add(normal, v3Scale(_getNormal(definition, position, 0.1), 0.25));
    normal = v3Scale(v3Normalize(normal), 0.1);

    return renderer->applyLightingToSurface(renderer, position, normal, definition->material);
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

    segment_count = _findSegments(definition, renderer, start, direction, detail, 20, definition->transparencydepth, max_length, &inside_length, &total_length, segments);
    for (i = 0; i < segment_count; i++)
    {
        col = _applyLayerLighting(definition, renderer, segments[i].start, detail);
        col.a = (segments[i].length >= definition->transparencydepth) ? 1.0 : (segments[i].length / definition->transparencydepth);
        colorMask(&result, &col);
    }

    result = renderer->applyAtmosphere(renderer, start, result);

    return result;
}

Color cloudsGetColor(CloudsDefinition* definition, Renderer* renderer, Vector3 start, Vector3 end)
{
    int i;
    Color layer_color, result;

    if (definition->nblayers < 1)
    {
        return COLOR_TRANSPARENT;
    }

    result = COLOR_TRANSPARENT;
    /* TODO Order layers */
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

Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    double inside_depth, total_depth, factor;
    CloudSegment segments[20];

    _optimizeSearchLimits(definition, &location, &light_location);

    _findSegments(definition, renderer, location, direction_to_light, 0.1, 20, definition->lighttraversal, v3Norm(v3Sub(light_location, location)), &inside_depth, &total_depth, segments);

    if (definition->lighttraversal < 0.0001)
    {
        factor = 0.0;
    }
    else
    {
        factor = inside_depth / definition->lighttraversal;
        if (factor > 1.0)
        {
            factor = 1.0;
        }
    }
    
    factor = 1.0 - (1.0 - definition->minimumlight) * factor;

    light.r = light.r * factor;
    light.g = light.g * factor;
    light.b = light.b * factor;

    return light;
}

Color cloudsFilterLight(CloudsDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    int i;
    for (i = 0; i < definition->nblayers; i++)
    {
        light = cloudsLayerFilterLight(definition->layers + i, renderer, light, location, light_location, direction_to_light);
    }
    return light;
}
