#include "BaseTestCase.h"

#include <cmath>
#include "renderer.h"
#include "clouds/public.h"
#include "clouds/clo_density.h"
#include "clouds/clo_walking.h"
#include "NoiseGenerator.h"

TEST(Clouds, Density)
{
    /* Setup */
    double x, y, z;
    CloudsLayerDefinition* layer;
    layer = (CloudsLayerDefinition*)cloudsGetLayerType().callback_create();

    /* Test default coverage (empty) */
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (y = -10.0; y < 10.0; y += 10.0)
        {
            for (z = -10.0; z < 10.0; z += 10.0)
            {
                ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, y, z)), 0.0);
            }
        }
    }

    /* Test coverage by altitude */
    layer->base_coverage = 1.0;
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);
    layer->base_coverage = 1.0;
    layer->_coverage_noise->forceValue(1.0);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 1.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.5);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.5);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }
    layer->base_coverage = 0.5;
    layer->_coverage_noise->forceValue(1.0);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 0.5);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.25);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.25);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }
    layer->base_coverage = 1.0;
    layer->_coverage_noise->forceValue(0.5);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 0.5);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.25);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.25);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ASSERT_DOUBLE_EQ(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }

    /* TODO Test fake renderer */

    /* TODO Test real renderer */

    /* Teardown */
    cloudsGetLayerType().callback_delete(layer);
}

TEST(Clouds, WalkingBoundaries)
{
    Vector3 start, end;
    int result;
    CloudsLayerDefinition* layer;
    layer = (CloudsLayerDefinition*)cloudsGetLayerType().callback_create();
    layer->base_coverage = 1.0;
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    /* Basic cases */
    start = v3(0.0, -3.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, 3.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, -1.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, 1.0, 0.0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, 0.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, 1.0, 0.0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, -1.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, 0.0, 0.0);

    /* Basic cases (inverted) */
    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, -3.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, 3.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, 1.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, -1.0, 0.0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, 1.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, 0.0, 0.0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, 0.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 0.0, -1.0, 0.0);

    /* Horizontal cases */
    start = v3(0.0, 2.0, 0.0);
    end = v3(10.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, 1.00001, 0.0);
    end = v3(10.0, 1.00001, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, -1.00001, 0.0);
    end = v3(10.0, -1.00001, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(10.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(10.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ASSERT_EQ(result, 1);
    ASSERT_VECTOR3_COORDS(start, 0.0, 0.0, 0.0);
    ASSERT_VECTOR3_COORDS(end, 10.0, 0.0, 0.0);

    cloudsGetLayerType().callback_delete(layer);
}

static double _getLayerDensitySinX(Renderer*, CloudsLayerDefinition*, Vector3 location)
{
    double density = sin(location.x * (2.0 * M_PI));
    return (density > 0.0) ? density : 0.0;
}

static double _getEdgeDensitySquared(Renderer*, CloudsLayerDefinition*, Vector3, double edge_density)
{
    return edge_density * edge_density;
}

TEST(Clouds, Walking)
{
    /* Init */
    CloudsLayerDefinition* layer;
    layer = (CloudsLayerDefinition*)cloudsGetLayerType().callback_create();
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    Renderer* renderer;
    renderer = rendererCreate();

    renderer->render_quality = 8;
    renderer->clouds->getLayerDensity = _getLayerDensitySinX;

    CloudsWalker* walker = cloudsCreateWalker(renderer, layer, v3(-0.4, 0.0, 0.0), v3(1.75, 0.0, 0.0));
    CloudWalkerStepInfo* segment;
    int result;

    /* First step */
    cloudsWalkerSetStepSize(walker, 0.3);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.3);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, 0.0);
    ASSERT_VECTOR3_COORDS(segment->start.location, -0.4, 0.0, 0.0);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, 0.3);
    ASSERT_VECTOR3_COORDS(segment->end.location, -0.1, 0.0, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.0);

    /* Second step */
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.3);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, 0.3);
    ASSERT_VECTOR3_COORDS(segment->start.location, -0.1, 0.0, 0.0);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, 0.6);
    ASSERT_VECTOR3_COORDS(segment->end.location, 0.2, 0.0, 0.0);
    ASSERT_GT(segment->end.global_density, 0.9);

    /* Order to refine second step around the entry point */
    cloudsWalkerOrderRefine(walker, 0.01);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_IN_RANGE(segment->length, 0.19, 0.20);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 0.40, 0.41);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 0.0, 0.01);
    ASSERT_GT(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, 0.6);
    ASSERT_VECTOR3_COORDS(segment->end.location, 0.2, 0.0, 0.0);
    ASSERT_GT(segment->end.global_density, 0.9);

    /* Third step, change step size */
    cloudsWalkerSetStepSize(walker, 0.4);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.4);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, 0.6);
    ASSERT_VECTOR3_COORDS(segment->start.location, 0.2, 0.0, 0.0);
    ASSERT_GT(segment->start.global_density, 0.9);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, 1.0);
    ASSERT_VECTOR3_COORDS(segment->end.location, 0.6, 0.0, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.0);

    /* Refine exit point */
    cloudsWalkerOrderRefine(walker, 0.001);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_IN_RANGE(segment->length, 0.3, 0.301);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, 0.6);
    ASSERT_VECTOR3_COORDS(segment->start.location, 0.2, 0.0, 0.0);
    ASSERT_GT(segment->start.global_density, 0.9);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 0.9, 0.901);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 0.5, 0.501);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.0);

    /* Find next entry point by skipping blank */
    cloudsWalkerSetVoidSkipping(walker, 1);
    cloudsWalkerSetStepSize(walker, 0.2);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.2);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 1.2, 1.4);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 0.8, 1.0);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 1.4, 1.6);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 1.0, 1.2);
    ASSERT_GT(segment->end.global_density, 0.0);

    /* Refine entry point */
    cloudsWalkerOrderRefine(walker, 0.01);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_TRUE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_IN_RANGE(segment->length, 0.0, 0.2);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 1.4, 1.41);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 1.0, 1.01);
    ASSERT_GT(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 1.41, 1.6);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 1.01, 1.2);
    ASSERT_GT(segment->end.global_density, 0.0);

    /* Subdivide entry for more detail */
    CloudWalkerStepInfo parent = *segment;
    cloudsWalkerOrderSubdivide(walker, 3);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_TRUE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, parent.length / 3.0);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, parent.start.distance_from_start);
    ASSERT_DOUBLE_EQ(segment->start.location.x, parent.start.location.x);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, parent.start.distance_from_start + segment->length);
    ASSERT_DOUBLE_EQ(segment->end.location.x, parent.start.location.x + segment->length);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_TRUE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, parent.length / 3.0);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, parent.start.distance_from_start + segment->length);
    ASSERT_DOUBLE_EQ(segment->start.location.x, parent.start.location.x + segment->length);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, parent.start.distance_from_start + 2.0 * segment->length);
    ASSERT_DOUBLE_EQ(segment->end.location.x, parent.start.location.x + 2.0 * segment->length);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_TRUE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, parent.length / 3.0);
    ASSERT_DOUBLE_EQ(segment->start.distance_from_start, parent.start.distance_from_start + 2.0 * segment->length);
    ASSERT_DOUBLE_EQ(segment->start.location.x, parent.start.location.x + 2.0 * segment->length);
    ASSERT_DOUBLE_EQ(segment->end.distance_from_start, parent.end.distance_from_start);
    ASSERT_DOUBLE_EQ(segment->end.location.x, parent.end.location.x);

    /* After subdividing, normal walking resumes */
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.2);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 1.41, 1.6);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 1.01, 1.2);
    ASSERT_GT(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 1.61, 1.8);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 1.21, 1.4);
    ASSERT_GT(segment->end.global_density, 0.0);

    /* Exiting cloud again */
    cloudsWalkerSetStepSize(walker, 0.3);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.3);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 1.61, 1.8);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 1.21, 1.4);
    ASSERT_GT(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 1.91, 2.1);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 1.5, 1.7);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.0);

    /* A step in the void without skipping */
    cloudsWalkerSetVoidSkipping(walker, 0);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_FALSE(segment->refined);
    ASSERT_FALSE(segment->subdivided);
    ASSERT_DOUBLE_EQ(segment->length, 0.3);
    ASSERT_DOUBLE_IN_RANGE(segment->start.distance_from_start, 1.91, 2.1);
    ASSERT_DOUBLE_IN_RANGE(segment->start.location.x, 1.5, 1.7);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_IN_RANGE(segment->end.distance_from_start, 2.21, 2.4);
    ASSERT_DOUBLE_IN_RANGE(segment->end.location.x, 1.8, 2.0);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.0);

    /* Walker reached the lookup segment's end, it should stop */
    result = cloudsWalkerPerformStep(walker);
    ASSERT_EQ(result, 0);

    /* Clean up */
    cloudsDeleteWalker(walker);

    cloudsGetLayerType().callback_delete(layer);
    rendererDelete(renderer);
}

TEST(Clouds, WalkingLocal)
{
    /* Init */
    CloudsLayerDefinition* layer;
    layer = (CloudsLayerDefinition*)cloudsGetLayerType().callback_create();
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    Renderer* renderer;
    renderer = rendererCreate();

    renderer->render_quality = 8;
    renderer->clouds->getLayerDensity = _getLayerDensitySinX;
    renderer->clouds->getEdgeDensity = _getEdgeDensitySquared;

    CloudsWalker* walker = cloudsCreateWalker(renderer, layer, v3(0.0, 0.0, 0.0), v3(1.0, 0.0, 0.0));
    CloudWalkerStepInfo* segment;
    int result;

    /* Test that local density is off by default */
    cloudsWalkerSetStepSize(walker, 0.3);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_DOUBLE_EQ(segment->length, 0.3);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->start.local_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.951056516295);
    ASSERT_DOUBLE_EQ(segment->end.local_density, 0.0);

    /* Test it's automatically enabled on subdivision */
    cloudsWalkerOrderSubdivide(walker, 2);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ASSERT_EQ(result, 1);
    ASSERT_DOUBLE_EQ(segment->length, 0.15);
    ASSERT_DOUBLE_EQ(segment->start.global_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->start.local_density, 0.0);
    ASSERT_DOUBLE_EQ(segment->end.global_density, 0.809016994375);
    ASSERT_DOUBLE_EQ(segment->end.local_density, 0.654508497187);
}
