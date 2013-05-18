#include "clo_walking.h"

#include "../renderer.h"

int cloudsOptimizeWalkingBounds(CloudsLayerDefinition* layer, Vector3* start, Vector3* end)
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

int cloudsGetLayerPrimarySegments(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end, int max_segments, CloudPrimarySegment* out_segments)
{
    int inside, segment_count;
    double step_length, segment_length;
    Vector3 diff, walker, segment_start;
    double render_precision, density;
    double diff_length, progress;

    if (max_segments <= 0)
    {
        return 0;
    }

    if (!cloudsOptimizeWalkingBounds(layer, &start, &end))
    {
        return 0;
    }

    diff = v3Sub(end, start);
    diff_length = v3Norm(diff);

    if (diff_length < 0.000001)
    {
        return 0;
    }

    render_precision = 1.005 - 0.01 * (double)(renderer->render_quality * renderer->render_quality);
    /*if (render_precision > max_total_length / 10.0)
    {
        render_precision = max_total_length / 10.0;
    }
    else if (render_precision < max_total_length / 10000.0)
    {
        render_precision = max_total_length / 10000.0;
    }*/

    segment_count = 0;
    segment_length = 0.0;
    density = renderer->clouds->getLayerDensity(renderer, layer, start);
    progress = 0.0;
    step_length = render_precision;
    inside = (density > 0.0);

    do
    {
        progress += step_length;
        walker = v3Add(start, v3Scale(diff, progress / diff_length));

        if (progress >= diff_length)
        {
            density = 0.0;
        }
        else
        {
            density = renderer->clouds->getLayerDensity(renderer, layer, walker);
        }

        if (density > 0.0)
        {
            if (inside)
            {
                /* inside the cloud */
                segment_length += step_length;
            }
            else
            {
                /* entering the cloud */
                segment_length = step_length;
                segment_start = v3Add(start, v3Scale(diff, (progress - step_length) / diff_length));
                /* TODO Refine entry position */

                inside = 1;
            }
        }
        else
        {
            if (inside)
            {
                /* exiting the cloud */
                segment_length += step_length;

                out_segments->enter = segment_start;
                out_segments->exit = walker;
                out_segments->length = segment_length;
                out_segments++;
                if (++segment_count >= max_segments)
                {
                    break;
                }
                /* TODO Refine exit position */

                inside = 0;
            }
        }
        /* step = v3Scale(direction, (info.distance_to_edge < render_precision) ? render_precision : info.distance_to_edge); */
    }
    while (inside || (walker.y <= layer->lower_altitude + layer->thickness + 0.001 && walker.y >= layer->lower_altitude - 0.001 && progress < diff_length));

    return segment_count;
}
