#include "clo_walking.h"

#include "../renderer.h"

/**
 * Control of the next walking order.
 */
typedef enum
{
    CLOUD_WALKING_CONTINUE,
    CLOUD_WALKING_STOP,
    CLOUD_WALKING_REFINE,
    CLOUD_WALKING_SUBDIVIDE
} CloudWalkingOrder;

/**
 * Additional info for walking orders.
 */
typedef struct
{
    CloudWalkingOrder order;
    double precision;
    int max_segments;
} CloudWalkingNextAction;

/*
 * Private structure for the walker.
 */
struct CloudsWalker
{
    Vector3 start;
    Vector3 end;
    Vector3 diff;

    double cursor;
    double max_length;
    double step_size;
    int skip_void;
    int local_density;

    int started;
    CloudWalkerStepInfo last_segment;

    int subdivision_current;
    int subdivision_count;
    CloudWalkerStepInfo subdivision_parent;

    CloudWalkingNextAction next_action;
};


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

    return 1;
}

CloudsWalker* cloudsCreateWalker(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 start, Vector3 end)
{
    CloudsWalker* result;

    result = (CloudsWalker*)malloc(sizeof (CloudsWalker));

    result->start = start;
    result->end = end;
    result->diff = v3Sub(end, start);
    result->max_length = v3Norm(result->diff);
    result->cursor = 0.0;
    result->step_size = 1.0;
    result->skip_void = 0;
    result->local_density = -1;

    result->started = 0;
    result->subdivision_count = 0;
    result->last_segment.renderer = renderer;
    result->last_segment.layer = layer;

    result->next_action.order = CLOUD_WALKING_CONTINUE;

    return result;
}

void cloudsDeleteWalker(CloudsWalker* walker)
{
    free(walker);
}

void cloudsWalkerSetStepSize(CloudsWalker* walker, double step)
{
    if (step > 0.0)
    {
        walker->step_size = step;
    }
    else
    {
        /* TODO Automatic settings (using rendering quality and cloud feature size) */
        walker->step_size = 5.0 / (double)walker->last_segment.renderer->render_quality;
    }
}

void cloudsWalkerToggleLocalDensity(CloudsWalker* walker, int enabled)
{
    walker->local_density = enabled;
}

void cloudsWalkerSetVoidSkipping(CloudsWalker* walker, int enabled)
{
    walker->skip_void = enabled;
}

static void _getPoint(CloudsWalker* walker, double cursor, CloudWalkerPoint* out_point)
{
    out_point->distance_from_start = cursor;
    out_point->location = v3Add(walker->start, v3Scale(walker->diff, out_point->distance_from_start / walker->max_length));

    Renderer* renderer = walker->last_segment.renderer;
    CloudsLayerDefinition* layer = walker->last_segment.layer;
    out_point->global_density = renderer->clouds->getLayerDensity(renderer, layer, out_point->location);

    if (walker->local_density > 0 || (walker->local_density < 0 && walker->subdivision_count > 0))
    {
        out_point->local_density = renderer->clouds->getEdgeDensity(renderer, layer, out_point->location, out_point->global_density);
    }
    else
    {
        out_point->local_density = 0.0;
    }
}

static void _refineSegment(CloudsWalker* walker, double start_cursor, double start_density, double end_cursor, double end_density, double precision, CloudWalkerPoint* result)
{
    CloudWalkerPoint middle;

    _getPoint(walker, (start_cursor + end_cursor) / 2.0, &middle);

    if (start_density == 0.0)
    {
        /* Looking for entry */
        if (middle.distance_from_start - start_cursor < precision)
        {
            *result = middle;
        }
        else if (middle.global_density == 0.0)
        {
            _refineSegment(walker, middle.distance_from_start, middle.global_density, end_cursor, end_density, precision, result);
        }
        else
        {
            _refineSegment(walker, start_cursor, start_density, middle.distance_from_start, middle.global_density, precision, result);
        }
    }
    else
    {
        /* Looking for exit */
        if (end_cursor - middle.distance_from_start < precision)
        {
            *result = middle;
        }
        else if (middle.global_density == 0.0)
        {
            _refineSegment(walker, start_cursor, start_density, middle.distance_from_start, middle.global_density, precision, result);
        }
        else
        {
            _refineSegment(walker, middle.distance_from_start, middle.global_density, end_cursor, end_density, precision, result);
        }
    }
}

int cloudsWalkerPerformStep(CloudsWalker* walker)
{
    int result = -1;

    if (!walker->started)
    {
        _getPoint(walker, 0.0, &walker->last_segment.end);
        walker->started = 1;
    }

    while (result < 0)
    {
        if (walker->next_action.order == CLOUD_WALKING_STOP || walker->cursor >= walker->max_length)
        {
            walker->next_action.order = CLOUD_WALKING_STOP;
            result = 0;
        }
        else if (walker->subdivision_count > 0)
        {
            if (walker->subdivision_current >= walker->subdivision_count)
            {
                /* Exit subdivision */
                walker->subdivision_count = 0;
                walker->last_segment = walker->subdivision_parent;
                walker->next_action.order = CLOUD_WALKING_CONTINUE;
                walker->cursor = walker->subdivision_parent.end.distance_from_start;

                /* Recursive call to progress */
                result = cloudsWalkerPerformStep(walker);
            }
            else
            {
                /* Continue subdivision */
                walker->last_segment.start = walker->last_segment.end;

                walker->cursor += walker->subdivision_parent.length / (double)walker->subdivision_count;

                _getPoint(walker, walker->cursor, &walker->last_segment.end);
                walker->last_segment.length = walker->subdivision_parent.length / (double)walker->subdivision_count;
                walker->last_segment.refined = 0;
                walker->last_segment.subdivided = walker->subdivision_count;

                walker->subdivision_current++;

                result = 1;
            }
        }
        else if (walker->next_action.order == CLOUD_WALKING_CONTINUE)
        {
            /* TODO Limit to lookup end */
            walker->last_segment.start = walker->last_segment.end;

            walker->cursor += walker->step_size;

            _getPoint(walker, walker->cursor, &walker->last_segment.end);
            walker->last_segment.length = walker->step_size;
            walker->last_segment.refined = 0;
            walker->last_segment.subdivided = 0;

            result = 1;
        }
        else if (walker->next_action.order == CLOUD_WALKING_REFINE)
        {
            /* Refine segment with dichotomy */
            _refineSegment(walker,
                           walker->last_segment.start.distance_from_start,
                           walker->last_segment.start.global_density,
                           walker->last_segment.end.distance_from_start,
                           walker->last_segment.end.global_density,
                           walker->next_action.precision,
                           (walker->last_segment.start.global_density == 0.0) ? (&walker->last_segment.start) : (&walker->last_segment.end));
            walker->last_segment.length = walker->last_segment.end.distance_from_start - walker->last_segment.start.distance_from_start;
            walker->last_segment.refined = 1;
            walker->last_segment.subdivided = 0;

            walker->next_action.order = CLOUD_WALKING_CONTINUE;

            result = 1;
        }
        else if (walker->next_action.order == CLOUD_WALKING_SUBDIVIDE)
        {
            /* Starting subdivision */
            walker->subdivision_count = walker->next_action.max_segments;
            walker->subdivision_current = 0;
            walker->subdivision_parent = walker->last_segment;
            walker->cursor = walker->subdivision_parent.start.distance_from_start;

            /* Copy parent segment start, to be used as first subdivided segment start */
            walker->last_segment.end = walker->subdivision_parent.start;

            /* Recursive call to get first subdivided segment */
            cloudsWalkerPerformStep(walker);

            result = 1;
        }
        else
        {
            /* Unknown order... */
            result = 0;
        }

        /* Check if we need to loop */
        if (result > 0 && walker->skip_void && walker->last_segment.start.global_density == 0.0 && walker->last_segment.end.global_density == 0.0)
        {
            /* Last segment is considered void, and skipping is enabled */
            result = -1;
        }
    }

    return result;
}

void cloudsWalkerOrderStop(CloudsWalker* walker)
{
    walker->next_action.order = CLOUD_WALKING_STOP;
}

void cloudsWalkerOrderRefine(CloudsWalker* walker, double precision)
{
    if (walker->subdivision_count == 0)
    {
        walker->next_action.order = CLOUD_WALKING_REFINE;
        walker->next_action.precision = precision;
    }
}

void cloudsWalkerOrderSubdivide(CloudsWalker* walker, double max_segments)
{
    if (walker->subdivision_count == 0)
    {
        walker->next_action.order = CLOUD_WALKING_SUBDIVIDE;
        walker->next_action.max_segments = max_segments;
    }
}

CloudWalkerStepInfo* cloudsWalkerGetLastSegment(CloudsWalker* walker)
{
    return &walker->last_segment;
}

void cloudsStartWalking(CloudsWalker* walker, FuncCloudsWalkingCallback callback, void* data)
{
    walker->last_segment.data = data;
    while (cloudsWalkerPerformStep(walker))
    {
        callback(walker);
    }
}
