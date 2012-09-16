#include "clouds.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "color.h"
#include "euclid.h"
#include "lighting.h"
#include "tools.h"
#include "shared/types.h"

#define CLOUDS_MAX_LAYERS 6
#define MAX_SEGMENT_COUNT 30

typedef struct
{
    Vector3 start;
    Vector3 end;
    double length;
} CloudSegment;

CloudsDefinition cloudsCreateDefinition()
{
    CloudsDefinition result;

    result.layers = layersCreate(cloudsGetLayerType(), CLOUDS_MAX_LAYERS);

    return result;
}

void cloudsDeleteDefinition(CloudsDefinition* definition)
{
    layersDelete(definition->layers);
}

void cloudsCopyDefinition(CloudsDefinition* source, CloudsDefinition* destination)
{
    layersCopy(source->layers, destination->layers);
}

void cloudsValidateDefinition(CloudsDefinition* definition)
{
    layersValidate(definition->layers);
}

void cloudsSave(PackStream* stream, CloudsDefinition* definition)
{
    layersSave(stream, definition->layers);
}

void cloudsLoad(PackStream* stream, CloudsDefinition* definition)
{
    layersLoad(stream, definition->layers);
}

static double _standardCoverageFunc(CloudsLayerDefinition* layer, Vector3 position)
{
    if (position.y < layer->lower_altitude || position.y >= layer->lower_altitude + layer->thickness)
    {
        return 0.0;
    }
    else
    {
        return layer->base_coverage * curveGetValue(layer->coverage_by_altitude, (position.y - layer->lower_altitude) / layer->thickness);
    }
}

CloudsLayerDefinition* cloudsLayerCreateDefinition()
{
    CloudsLayerDefinition* result;
    
    result = malloc(sizeof(CloudsLayerDefinition));

    result->lower_altitude = 4.0;
    result->thickness = 6.0;
    result->coverage_by_altitude = curveCreate();
    curveQuickAddPoint(result->coverage_by_altitude, 0.0, 0.0);
    curveQuickAddPoint(result->coverage_by_altitude, 0.3, 1.0);
    curveQuickAddPoint(result->coverage_by_altitude, 0.5, 1.0);
    curveQuickAddPoint(result->coverage_by_altitude, 1.0, 0.0);
    result->material.base.r = 0.7;
    result->material.base.g = 0.7;
    result->material.base.b = 0.7;
    result->material.base.a = 1.0;
    result->material.reflection = 0.3;
    result->material.shininess = 0.8;
    result->hardness = 0.25;
    result->transparencydepth = 1.5;
    result->lighttraversal = 7.0;
    result->minimumlight = 0.4;
    result->shape_scaling = 10.0;
    result->edge_scaling = 0.8;
    result->edge_length = 0.3;
    result->base_coverage = 0.4;
    result->shape_noise = noiseCreateGenerator();
    noiseAddLevelsSimple(result->shape_noise, 2, 1.0, 1.0);
    noiseSetFunctionParams(result->shape_noise, NOISE_FUNCTION_SIMPLEX, 0.3);
    result->edge_noise = noiseCreateGenerator();
    noiseAddLevelsSimple(result->edge_noise, 8, 1.0, 1.0);
    noiseSetFunctionParams(result->edge_noise, NOISE_FUNCTION_SIMPLEX, 0.5);
    
    result->_custom_coverage = _standardCoverageFunc;

    return result;
}

void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition)
{
    curveDelete(definition->coverage_by_altitude);
    noiseDeleteGenerator(definition->shape_noise);
    noiseDeleteGenerator(definition->edge_noise);
    free(definition);
}

void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination)
{
    CloudsLayerDefinition temp;
    
    temp = *destination;
    *destination = *source;
    
    destination->shape_noise = temp.shape_noise;
    noiseCopy(source->shape_noise, destination->shape_noise);

    destination->edge_noise = temp.edge_noise;
    noiseCopy(source->edge_noise, destination->edge_noise);

    destination->coverage_by_altitude = temp.coverage_by_altitude;
    curveCopy(source->coverage_by_altitude, destination->coverage_by_altitude);
}

void cloudsLayerValidateDefinition(CloudsLayerDefinition* definition)
{
    if (definition->shape_scaling < 0.0001)
    {
        definition->shape_scaling = 0.00001;
    }
    if (definition->edge_scaling < 0.0001)
    {
        definition->edge_scaling = 0.00001;
    }
    if (definition->_custom_coverage == NULL)
    {
        definition->_custom_coverage = _standardCoverageFunc;
    }
}

void _cloudsLayerSave(PackStream* stream, CloudsLayerDefinition* layer)
{
    packWriteDouble(stream, &layer->lower_altitude);
    packWriteDouble(stream, &layer->thickness);
    curveSave(stream, layer->coverage_by_altitude);
    noiseSaveGenerator(stream, layer->shape_noise);
    noiseSaveGenerator(stream, layer->edge_noise);
    materialSave(stream, &layer->material);
    packWriteDouble(stream, &layer->hardness);
    packWriteDouble(stream, &layer->transparencydepth);
    packWriteDouble(stream, &layer->lighttraversal);
    packWriteDouble(stream, &layer->minimumlight);
    packWriteDouble(stream, &layer->shape_scaling);
    packWriteDouble(stream, &layer->edge_scaling);
    packWriteDouble(stream, &layer->edge_length);
    packWriteDouble(stream, &layer->base_coverage);
}

void _cloudsLayerLoad(PackStream* stream, CloudsLayerDefinition* layer)
{
    packReadDouble(stream, &layer->lower_altitude);
    packReadDouble(stream, &layer->thickness);
    curveLoad(stream, layer->coverage_by_altitude);
    noiseLoadGenerator(stream, layer->shape_noise);
    noiseLoadGenerator(stream, layer->edge_noise);
    materialLoad(stream, &layer->material);
    packReadDouble(stream, &layer->hardness);
    packReadDouble(stream, &layer->transparencydepth);
    packReadDouble(stream, &layer->lighttraversal);
    packReadDouble(stream, &layer->minimumlight);
    packReadDouble(stream, &layer->shape_scaling);
    packReadDouble(stream, &layer->edge_scaling);
    packReadDouble(stream, &layer->edge_length);
    packReadDouble(stream, &layer->base_coverage);
}

LayerType cloudsGetLayerType()
{
    LayerType result;
    
    result.callback_create = (LayerCallbackCreate)cloudsLayerCreateDefinition;
    result.callback_delete = (LayerCallbackDelete)cloudsLayerDeleteDefinition;
    result.callback_copy = (LayerCallbackCopy)cloudsLayerCopyDefinition;
    result.callback_validate = (LayerCallbackValidate)cloudsLayerValidateDefinition;
    result.callback_save = (LayerCallbackSave)_cloudsLayerSave;
    result.callback_load = (LayerCallbackLoad)_cloudsLayerLoad;
    
    return result;
}

static inline double _getDistanceToBorder(CloudsLayerDefinition* layer, Vector3 position)
{
    double density, coverage, val;

    val = noiseGet3DTotal(layer->shape_noise, position.x / layer->shape_scaling, position.y / layer->shape_scaling, position.z / layer->shape_scaling) / noiseGetMaxValue(layer->shape_noise);
    coverage = layer->_custom_coverage(layer, position);
    density = 0.5 * val - 0.5 + coverage;
    
    if (density <= 0.0)
    {
        /* outside the main shape */
        return density * layer->shape_scaling;
    }
    else
    {
        /* inside the main shape, using edge noise */
        density /= coverage;
        if (density < layer->edge_length)
        {
            density /= layer->edge_length;
            
            val = 0.5 * noiseGet3DTotal(layer->edge_noise, position.x / layer->edge_scaling, position.y / layer->edge_scaling, position.z / layer->edge_scaling) / noiseGetMaxValue(layer->edge_noise);
            val = val - 0.5 + density;

            return val * (density * coverage * layer->shape_scaling + (1.0 - density) * layer->edge_scaling);
        }
        else
        {
            return density * coverage * layer->shape_scaling;
        }
    }
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
    
    if (start->y > layer->lower_altitude + layer->thickness)
    {
        if (end->y >= layer->lower_altitude + layer->thickness)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
            if (end->y < layer->lower_altitude)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude - end->y) / diff.y));
            }
        }
    }
    else if (start->y < layer->lower_altitude)
    {
        if (end->y <= layer->lower_altitude)
        {
            return 0;
        }
        else
        {
            diff = v3Sub(*end, *start);
            *start = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
            if (end->y >= layer->lower_altitude + layer->thickness)
            {
                *end = v3Add(*end, v3Scale(diff, (layer->lower_altitude + layer->thickness - end->y) / diff.y));
            }
        }
    }
    else /* start is inside layer */
    {
        diff = v3Sub(*end, *start);
        if (end->y > layer->lower_altitude + layer->thickness)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude + layer->thickness - start->y) / diff.y));
        }
        else if (end->y < layer->lower_altitude)
        {
            *end = v3Add(*start, v3Scale(diff, (layer->lower_altitude - start->y) / diff.y));
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

    render_precision = 15.2 - 1.5 * (double)renderer->render_quality;
    render_precision = render_precision * definition->shape_scaling / 150.0;
    if (render_precision > max_total_length / 10.0)
    {
        render_precision = max_total_length / 10.0;
    }
    else if (render_precision < max_total_length / 2000.0)
    {
        render_precision = max_total_length / 2000.0;
    }

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

        if (current_total_length >= max_total_length || current_inside_length > max_inside_length)
        {
            noise_distance = 0.0;
        }

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
    } while (inside || (walker.y <= definition->lower_altitude + definition->thickness + 0.001 && walker.y >= definition->lower_altitude - 0.001 && current_total_length < max_total_length && current_inside_length < max_inside_length));

    *total_length = current_total_length;
    *inside_length = current_inside_length;
    return segment_count;
}

static Color _applyLayerLighting(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 position, double detail)
{
    Vector3 normal;
    Color col1, col2;
    LightStatus light;

    normal = _getNormal(definition, position, 3.0);
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, position, 2.0));
        normal = v3Add(normal, _getNormal(definition, position, 1.0));
    }
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, position, 0.5));
    }
    normal = v3Scale(v3Normalize(normal), definition->hardness);
    
    renderer->getLightStatus(renderer, &light, position);
    col1 = renderer->applyLightStatus(renderer, &light, position, normal, definition->material);
    col2 = renderer->applyLightStatus(renderer, &light, position, v3Scale(normal, -1.0), definition->material);
    
    col1.r = (col1.r + col2.r) / 2.0;
    col1.g = (col1.g + col2.g) / 2.0;
    col1.b = (col1.b + col2.b) / 2.0;
    col1.a = (col1.a + col2.a) / 2.0;

    return col1;
}

Color cloudsApplyLayer(CloudsLayerDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end)
{
    int i, segment_count;
    double max_length, detail, total_length, inside_length;
    Vector3 direction;
    Color col;
    CloudSegment segments[MAX_SEGMENT_COUNT];

    if (!_optimizeSearchLimits(definition, &start, &end))
    {
        return base;
    }

    direction = v3Sub(end, start);
    max_length = v3Norm(direction);
    direction = v3Normalize(direction);

    detail = renderer->getPrecision(renderer, start) / definition->shape_scaling;

    segment_count = _findSegments(definition, renderer, start, direction, detail, MAX_SEGMENT_COUNT, definition->transparencydepth * (double)renderer->render_quality, max_length, &inside_length, &total_length, segments);
    for (i = segment_count - 1; i >= 0; i--)
    {
        col = _applyLayerLighting(definition, renderer, segments[i].start, detail);
        col.a = 1.0;
        col = renderer->applyAtmosphere(renderer, start, col);
        col.a = (segments[i].length >= definition->transparencydepth) ? 1.0 : (segments[i].length / definition->transparencydepth);
        colorMask(&base, &col);
    }
    if (inside_length >= definition->transparencydepth)
    {
        col.a = 1.0;
    }

    return base;
}

/*static int _cmpLayer(const void* layer1, const void* layer2)
{
    return (((CloudsLayerDefinition*)layer1)->lower_altitude > ((CloudsLayerDefinition*)layer2)->lower_altitude) ? -1 : 1;
}*/

Color cloudsApply(CloudsDefinition* definition, Color base, Renderer* renderer, Vector3 start, Vector3 end)
{
    int i, n;

    n = layersCount(definition->layers);
    if (n < 1)
    {
        return base;
    }

    /* TODO Iter layers in sorted order */
    for (i = 0; i < n; i++)
    {
        base = cloudsApplyLayer(layersGetLayer(definition->layers, i), base, renderer, start, end);
    }

    return base;
}

Color cloudsLayerFilterLight(CloudsLayerDefinition* definition, Renderer* renderer, Color light, Vector3 location, Vector3 light_location, Vector3 direction_to_light)
{
    double inside_depth, total_depth, factor;
    CloudSegment segments[MAX_SEGMENT_COUNT];

    _optimizeSearchLimits(definition, &location, &light_location);

    _findSegments(definition, renderer, location, direction_to_light, 0.1, MAX_SEGMENT_COUNT, definition->lighttraversal, v3Norm(v3Sub(light_location, location)), &inside_depth, &total_depth, segments);

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
    int i, n;
    
    /* TODO Iter layers in sorted order */
    n = layersCount(definition->layers);
    for (i = 0; i < n; i++)
    {
        light = cloudsLayerFilterLight(layersGetLayer(definition->layers, i), renderer, light, location, light_location, direction_to_light);
    }
    return light;
}
