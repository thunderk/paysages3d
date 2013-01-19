#include "clouds.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "tools/color.h"
#include "tools/euclid.h"
#include "tools.h"

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
        return layer->base_coverage * curveGetValue(layer->_coverage_by_altitude, (position.y - layer->lower_altitude) / layer->thickness);
    }
}

CloudsLayerDefinition* cloudsLayerCreateDefinition()
{
    CloudsLayerDefinition* result;

    result = malloc(sizeof(CloudsLayerDefinition));
    result->_coverage_by_altitude = curveCreate();
    result->_shape_noise = noiseCreateGenerator();
    result->_edge_noise = noiseCreateGenerator();

    result->_custom_coverage = _standardCoverageFunc;

    cloudsLayerAutoPreset(result, CLOUDS_PRESET_CIRRUS);

    return result;
}

void cloudsLayerDeleteDefinition(CloudsLayerDefinition* definition)
{
    curveDelete(definition->_coverage_by_altitude);
    noiseDeleteGenerator(definition->_shape_noise);
    noiseDeleteGenerator(definition->_edge_noise);
    free(definition);
}

void cloudsLayerAutoPreset(CloudsLayerDefinition* definition, CloudsPreset preset)
{
    definition->material.base.r = 0.7;
    definition->material.base.g = 0.7;
    definition->material.base.b = 0.7;
    definition->material.base.a = 1.0;

    switch (preset)
    {
        case CLOUDS_PRESET_CIRRUS:
            definition->type = CLOUDS_TYPE_CIRRUS;
            definition->lower_altitude = 25.0;
            definition->thickness = 2.0;
            definition->material.reflection = 0.4;
            definition->material.shininess = 0.5;
            definition->hardness = 0.0;
            definition->transparencydepth = 3.0;
            definition->lighttraversal = 10.0;
            definition->minimumlight = 0.6;
            definition->shape_scaling = 8.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 0.8;
            definition->base_coverage = 0.6;
            break;
        case CLOUDS_PRESET_CUMULUS:
            definition->type = CLOUDS_TYPE_CUMULUS;
            definition->lower_altitude = 15.0;
            definition->thickness = 15.0;
            definition->material.reflection = 0.5;
            definition->material.shininess = 1.2;
            definition->hardness = 0.25;
            definition->transparencydepth = 1.5;
            definition->lighttraversal = 8.0;
            definition->minimumlight = 0.4;
            definition->shape_scaling = 20.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 0.0;
            definition->base_coverage = 0.7;
            break;
        case CLOUDS_PRESET_STRATOCUMULUS:
            definition->type = CLOUDS_TYPE_STRATOCUMULUS;
            definition->lower_altitude = 5.0;
            definition->thickness = 6.0;
            definition->material.reflection = 0.3;
            definition->material.shininess = 0.8;
            definition->hardness = 0.25;
            definition->transparencydepth = 1.5;
            definition->lighttraversal = 7.0;
            definition->minimumlight = 0.4;
            definition->shape_scaling = 10.0;
            definition->edge_scaling = 0.8;
            definition->edge_length = 0.3;
            definition->base_coverage = 0.4;
            break;
        case CLOUDS_PRESET_STRATUS:
            definition->type = CLOUDS_TYPE_STRATUS;
            definition->lower_altitude = 3.0;
            definition->thickness = 4.0;
            definition->material.reflection = 0.1;
            definition->material.shininess = 0.8;
            definition->hardness = 0.1;
            definition->transparencydepth = 3.0;
            definition->lighttraversal = 10.0;
            definition->minimumlight = 0.6;
            definition->shape_scaling = 8.0;
            definition->edge_scaling = 2.0;
            definition->edge_length = 1.0;
            definition->base_coverage = 0.4;
            break;
        default:
            break;
    }

    cloudsLayerValidateDefinition(definition);
}

void cloudsLayerCopyDefinition(CloudsLayerDefinition* source, CloudsLayerDefinition* destination)
{
    CloudsLayerDefinition temp;

    temp = *destination;
    *destination = *source;

    destination->_shape_noise = temp._shape_noise;
    noiseCopy(source->_shape_noise, destination->_shape_noise);

    destination->_edge_noise = temp._edge_noise;
    noiseCopy(source->_edge_noise, destination->_edge_noise);

    destination->_coverage_by_altitude = temp._coverage_by_altitude;
    curveCopy(source->_coverage_by_altitude, destination->_coverage_by_altitude);
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

    curveClear(definition->_coverage_by_altitude);
    noiseClearLevels(definition->_shape_noise);
    noiseClearLevels(definition->_edge_noise);

    switch (definition->type)
    {
        case CLOUDS_TYPE_CIRRUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.5, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            noiseAddLevelsSimple(definition->_shape_noise, 3, 1.0, 1.0);
            noiseSetFunctionParams(definition->_shape_noise, NOISE_FUNCTION_SIMPLEX, 0.0);
            noiseAddLevelsSimple(definition->_edge_noise, 4, 1.0, 1.0);
            noiseSetFunctionParams(definition->_edge_noise, NOISE_FUNCTION_SIMPLEX, -0.2);
            break;
        case CLOUDS_TYPE_CUMULUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.1, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.4, 0.8);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.7, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            noiseAddLevelsSimple(definition->_shape_noise, 7, 1.0, 1.0);
            noiseSetFunctionParams(definition->_shape_noise, NOISE_FUNCTION_SIMPLEX, 0.4);
            break;
        case CLOUDS_TYPE_STRATOCUMULUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.2, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.5, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            noiseAddLevelsSimple(definition->_shape_noise, 2, 1.0, 1.0);
            noiseSetFunctionParams(definition->_shape_noise, NOISE_FUNCTION_SIMPLEX, 0.3);
            noiseAddLevelsSimple(definition->_edge_noise, 8, 1.0, 1.0);
            noiseSetFunctionParams(definition->_edge_noise, NOISE_FUNCTION_SIMPLEX, 0.5);
            break;
        case CLOUDS_TYPE_STRATUS:
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.0, 0.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.2, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 0.8, 1.0);
            curveQuickAddPoint(definition->_coverage_by_altitude, 1.0, 0.0);
            noiseAddLevelsSimple(definition->_shape_noise, 3, 1.0, 1.0);
            noiseSetFunctionParams(definition->_shape_noise, NOISE_FUNCTION_SIMPLEX, -0.3);
            noiseAddLevelsSimple(definition->_edge_noise, 4, 1.0, 1.0);
            noiseSetFunctionParams(definition->_edge_noise, NOISE_FUNCTION_SIMPLEX, -0.5);
            break;
        default:
            break;
    }
}

void _cloudsLayerSave(PackStream* stream, CloudsLayerDefinition* layer)
{
    int clouds_type = (int)layer->type;

    packWriteInt(stream, &clouds_type);
    packWriteDouble(stream, &layer->lower_altitude);
    packWriteDouble(stream, &layer->thickness);
    curveSave(stream, layer->_coverage_by_altitude);
    noiseSaveGenerator(stream, layer->_shape_noise);
    noiseSaveGenerator(stream, layer->_edge_noise);
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
    int clouds_type;

    packReadInt(stream, &clouds_type);
    layer->type = (CloudsType)clouds_type;
    packReadDouble(stream, &layer->lower_altitude);
    packReadDouble(stream, &layer->thickness);
    materialLoad(stream, &layer->material);
    packReadDouble(stream, &layer->hardness);
    packReadDouble(stream, &layer->transparencydepth);
    packReadDouble(stream, &layer->lighttraversal);
    packReadDouble(stream, &layer->minimumlight);
    packReadDouble(stream, &layer->shape_scaling);
    packReadDouble(stream, &layer->edge_scaling);
    packReadDouble(stream, &layer->edge_length);
    packReadDouble(stream, &layer->base_coverage);

    cloudsLayerValidateDefinition(layer);
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

    val = noiseGet3DTotal(layer->_shape_noise, position.x / layer->shape_scaling, position.y / layer->shape_scaling, position.z / layer->shape_scaling) / noiseGetMaxValue(layer->_shape_noise);
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

            val = 0.5 * noiseGet3DTotal(layer->_edge_noise, position.x / layer->edge_scaling, position.y / layer->edge_scaling, position.z / layer->edge_scaling) / noiseGetMaxValue(layer->_edge_noise);
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

    render_precision = 1.005 - 0.01 * (double)(renderer->render_quality * renderer->render_quality);
    if (render_precision > max_total_length / 10.0)
    {
        render_precision = max_total_length / 10.0;
    }
    else if (render_precision < max_total_length / 10000.0)
    {
        render_precision = max_total_length / 10000.0;
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
    LightStatus* lighting;

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

    lighting = lightingCreateStatus(renderer->lighting, position, renderer->camera_location);
    renderer->atmosphere->getLightingStatus(renderer, lighting, normal, 0);
    col1 = lightingApplyStatus(lighting, normal, &definition->material);
    col2 = lightingApplyStatus(lighting, v3Scale(normal, -1.0), &definition->material);
    lightingDeleteStatus(lighting);

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
        col = renderer->atmosphere->applyAerialPerspective(renderer, start, col);
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
