#include "testing/common.h"

#include "rendering/renderer.h"
#include "rendering/tools.h"
#include "rendering/clouds/public.h"
#include "rendering/clouds/clo_density.h"
#include "rendering/clouds/clo_walking.h"

START_TEST(test_clouds_density)
{
    /* Setup */
    double x, y, z;
    CloudsLayerDefinition* layer;
    layer = cloudsGetLayerType().callback_create();

    /* Test default coverage (empty) */
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (y = -10.0; y < 10.0; y += 10.0)
        {
            for (z = -10.0; z < 10.0; z += 10.0)
            {
                ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, y, z)), 0.0);
            }
        }
    }

    /* Test coverage by altitude */
    layer->base_coverage = 1.0;
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);
    layer->base_coverage = 1.0;
    noiseForceValue(layer->_coverage_noise, 1.0);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 1.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.5);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.5);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }
    layer->base_coverage = 0.5;
    noiseForceValue(layer->_coverage_noise, 1.0);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 0.5);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.25);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.25);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }
    layer->base_coverage = 1.0;
    noiseForceValue(layer->_coverage_noise, 0.5);
    for (x = -10.0; x < 10.0; x += 10.0)
    {
        for (z = -10.0; z < 10.0; z += 10.0)
        {
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.0, z)), 0.5);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 0.5, z)), 0.25);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, 1.5, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -0.5, z)), 0.25);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.0, z)), 0.0);
            ck_assert_double_eq(cloudsGetLayerCoverage(layer, v3(x, -1.5, z)), 0.0);
        }
    }

    /* TODO Test fake renderer */

    /* TODO Test real renderer */

    /* Teardown */
    cloudsGetLayerType().callback_delete(layer);
}
END_TEST

START_TEST(test_clouds_walking_boundaries)
{
    Vector3 start, end;
    int result;
    CloudsLayerDefinition* layer;
    layer = cloudsGetLayerType().callback_create();
    layer->base_coverage = 1.0;
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    /* Basic cases */
    start = v3(0.0, -3.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, 3.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, -1.0, 0.0);
    ck_assert_vector_values(end, 0.0, 1.0, 0.0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, 0.0, 0.0);
    ck_assert_vector_values(end, 0.0, 1.0, 0.0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, -1.0, 0.0);
    ck_assert_vector_values(end, 0.0, 0.0, 0.0);

    /* Basic cases (inverted) */
    start = v3(0.0, -2.0, 0.0);
    end = v3(0.0, -3.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, 3.0, 0.0);
    end = v3(0.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, 1.0, 0.0);
    ck_assert_vector_values(end, 0.0, -1.0, 0.0);

    start = v3(0.0, 2.0, 0.0);
    end = v3(0.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, 1.0, 0.0);
    ck_assert_vector_values(end, 0.0, 0.0, 0.0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(0.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, 0.0, 0.0);
    ck_assert_vector_values(end, 0.0, -1.0, 0.0);

    /* Horizontal cases */
    start = v3(0.0, 2.0, 0.0);
    end = v3(10.0, 2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, 1.00001, 0.0);
    end = v3(10.0, 1.00001, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, -1.00001, 0.0);
    end = v3(10.0, -1.00001, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, -2.0, 0.0);
    end = v3(10.0, -2.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 0);

    start = v3(0.0, 0.0, 0.0);
    end = v3(10.0, 0.0, 0.0);
    result = cloudsOptimizeWalkingBounds(layer, &start, &end);
    ck_assert_int_eq(result, 1);
    ck_assert_vector_values(start, 0.0, 0.0, 0.0);
    ck_assert_vector_values(end, 10.0, 0.0, 0.0);

    cloudsGetLayerType().callback_delete(layer);
}
END_TEST

static double _getLayerDensitySinX(Renderer* renderer, CloudsLayerDefinition* layer, Vector3 location)
{
    UNUSED(renderer);
    UNUSED(layer);
    double density = sin(location.x * (2.0 * M_PI));
    return (density > 0.0) ? density : 0.0;
}

START_TEST(test_clouds_walking)
{
    /* Init */
    CloudsLayerDefinition* layer;
    layer = cloudsGetLayerType().callback_create();
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    Renderer* renderer;
    renderer = rendererCreate();

    renderer->render_quality = 8;
    renderer->clouds->getLayerDensity = _getLayerDensitySinX;

    CloudsWalker* walker = cloudsCreateWalker(renderer, layer, v3(-0.4, 0.0, 0.0), v3(10.0, 0.0, 0.0));
    CloudWalkerStepInfo* segment;
    int result;

    /* First step */
    cloudsWalkerSetStepSize(walker, 0.3);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ck_assert_int_eq(result, 1);
    ck_assert_false(segment->refined);
    ck_assert_double_eq(segment->length, 0.3);
    ck_assert_double_eq(segment->start.distance_from_start, 0.0);
    ck_assert_vector_values(segment->start.location, -0.4, 0.0, 0.0);
    ck_assert_double_eq(segment->start.global_density, 0.0);
    ck_assert_double_eq(segment->end.distance_from_start, 0.3);
    ck_assert_vector_values(segment->end.location, -0.1, 0.0, 0.0);
    ck_assert_double_eq(segment->end.global_density, 0.0);

    /* Second step */
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ck_assert_int_eq(result, 1);
    ck_assert_false(segment->refined);
    ck_assert_double_eq(segment->length, 0.3);
    ck_assert_double_eq(segment->start.distance_from_start, 0.3);
    ck_assert_vector_values(segment->start.location, -0.1, 0.0, 0.0);
    ck_assert_double_eq(segment->start.global_density, 0.0);
    ck_assert_double_eq(segment->end.distance_from_start, 0.6);
    ck_assert_vector_values(segment->end.location, 0.2, 0.0, 0.0);
    ck_assert_double_gt(segment->end.global_density, 0.9);

    /* Order to refine second step around the entry point */
    cloudsWalkerOrderRefine(walker, 0.01);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ck_assert_int_eq(result, 1);
    ck_assert_true(segment->refined);
    ck_assert_double_in_range(segment->length, 0.19, 0.20);
    ck_assert_double_in_range(segment->start.distance_from_start, 0.40, 0.41);
    ck_assert_double_in_range(segment->start.location.x, 0.0, 0.01);
    ck_assert_double_gt(segment->start.global_density, 0.0);
    ck_assert_double_eq(segment->end.distance_from_start, 0.6);
    ck_assert_vector_values(segment->end.location, 0.2, 0.0, 0.0);
    ck_assert_double_gt(segment->end.global_density, 0.9);

    /* Third step, change step size */
    cloudsWalkerSetStepSize(walker, 0.4);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ck_assert_int_eq(result, 1);
    ck_assert_false(segment->refined);
    ck_assert_double_eq(segment->length, 0.4);
    ck_assert_double_eq(segment->start.distance_from_start, 0.6);
    ck_assert_vector_values(segment->start.location, 0.2, 0.0, 0.0);
    ck_assert_double_gt(segment->start.global_density, 0.9);
    ck_assert_double_eq(segment->end.distance_from_start, 1.0);
    ck_assert_vector_values(segment->end.location, 0.6, 0.0, 0.0);
    ck_assert_double_eq(segment->end.global_density, 0.0);

    /* Refine exit point */
    cloudsWalkerOrderRefine(walker, 0.001);
    result = cloudsWalkerPerformStep(walker);
    segment = cloudsWalkerGetLastSegment(walker);
    ck_assert_int_eq(result, 1);
    ck_assert_true(segment->refined);
    ck_assert_double_in_range(segment->length, 0.3, 0.301);
    ck_assert_double_eq(segment->start.distance_from_start, 0.6);
    ck_assert_vector_values(segment->start.location, 0.2, 0.0, 0.0);
    ck_assert_double_gt(segment->start.global_density, 0.9);
    ck_assert_double_in_range(segment->end.distance_from_start, 0.9, 0.901);
    ck_assert_double_in_range(segment->end.location.x, 0.5, 0.501);
    ck_assert_double_lt(segment->end.global_density, 0.1);

    /* Clean up */
    cloudsDeleteWalker(walker);

    cloudsGetLayerType().callback_delete(layer);
    rendererDelete(renderer);
}
END_TEST

TEST_CASE(clouds,
          test_clouds_density,
          test_clouds_walking_boundaries,
          test_clouds_walking)
