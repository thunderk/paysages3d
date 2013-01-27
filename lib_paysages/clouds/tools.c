#include "private.h"

/*
 * Clouds tools.
 */

#include "../renderer.h"
#include "../tools.h"

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

static inline double _getDistanceToBorder(CloudsLayerDefinition* layer, Vector3 position)
{
    double density, coverage, val;

    val = noiseGet3DTotal(layer->_shape_noise, position.x / layer->shape_scaling, position.y / layer->shape_scaling, position.z / layer->shape_scaling) / 0.5;
    coverage = _standardCoverageFunc(layer, position);
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

            val = 0.5 * noiseGet3DTotal(layer->_edge_noise, position.x / layer->edge_scaling, position.y / layer->edge_scaling, position.z / layer->edge_scaling) / 0.5;
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

static Color _applyLayerLighting(CloudsLayerDefinition* definition, Renderer* renderer, Vector3 location, double detail)
{
    Vector3 normal;
    Color col1, col2;
    LightStatus* lighting;

    normal = _getNormal(definition, location, 3.0);
    if (renderer->render_quality > 3)
    {
        normal = v3Add(normal, _getNormal(definition, location, 2.0));
        normal = v3Add(normal, _getNormal(definition, location, 1.0));
    }
    if (renderer->render_quality > 5)
    {
        normal = v3Add(normal, _getNormal(definition, location, 0.5));
    }
    if (renderer->render_quality > 8)
    {
        normal = v3Add(normal, _getNormal(definition, location, 0.75));
        normal = v3Add(normal, _getNormal(definition, location, 1.25));
        normal = v3Add(normal, _getNormal(definition, location, 2.5));
    }
    normal = v3Scale(v3Normalize(normal), definition->hardness);

    return renderer->applyLightingToSurface(renderer, location, normal, &definition->material);

    lighting = lightingCreateStatus(renderer->lighting, location, renderer->camera_location);
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

/*
 * Get the coverage factor at the given location [0.0;1.0].
 * 0.0 means no cloud is present.
 * 1.0 means full layer.
 */
static inline double _getLayerCoverage(CloudsLayerDefinition* layer, double x, double z)
{
    return sin(x) * cos(z);
}

/*
 * Get the local density factor at the given location [0.0;1.0].
 * 0.0 means no cloud is present.
 * 1.0 means full density (deep inside cloud).
 */
static inline double _getLayerDensity(CloudsLayerDefinition* layer, Vector3 location, double coverage)
{
    return 1.0;
}

CloudsInfo cloudsGetLayerInfo(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location)
{
    CloudsInfo result;

    UNUSED(renderer);

    result.density = 0.0;
    result.distance_to_edge = 1.0;

    /* Get coverage info */
    double coverage = _getLayerCoverage(layer, location.x, location.z);
    if (coverage <= 0.0)
    {
        /* TODO Distance to edge */
    }
    else
    {
        /* Apply altitude to coverage */
        coverage *= curveGetValue(layer->_coverage_by_altitude, (location.y - layer->lower_altitude) / layer->thickness);
        if (coverage <= 0.0)
        {
            /* TODO Distance to edge */
        }
        else
        {
            /* Get local density */
            result.density = _getLayerDensity(layer, location, coverage);
            if (result.density <= 0)
            {
                /* TODO Distance to edge */
            }
            else
            {
                /* TODO Distance to edge */
            }
        }
    }

    result.inside = (result.density > 0.0);

    return result;
}

