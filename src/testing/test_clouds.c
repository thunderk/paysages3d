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

START_TEST(test_clouds_primary_segments)
{
    int segment_count, i;
    CloudPrimarySegment segments[10];

    CloudsLayerDefinition* layer;
    layer = cloudsGetLayerType().callback_create();
    layer->lower_altitude = -1.0;
    layer->thickness = 2.0;
    cloudsGetLayerType().callback_validate(layer);

    Renderer* renderer;
    renderer = rendererCreate();

    renderer->render_quality = 8;
    renderer->clouds->getLayerDensity = _getLayerDensitySinX;

    segment_count = cloudsGetLayerPrimarySegments(renderer, layer, v3(-0.4, 0.0, 0.0), v3(1.9, 0.0, 0.0), 10, segments);
    ck_assert_int_eq(segment_count, 2);
    for (i = 0; i < segment_count; i++)
    {
        ck_assert_double_eq(segments[i].enter.y, 0.0);
        ck_assert_double_eq(segments[i].enter.z, 0.0);
        ck_assert_double_eq(segments[i].exit.y, 0.0);
        ck_assert_double_eq(segments[i].exit.z, 0.0);
    }
    ck_assert_double_in_range(segments[0].enter.x, -0.5, 0.0);
    ck_assert_double_in_range(segments[0].exit.x, 0.5, 1.0);
    ck_assert_double_in_range(segments[0].length, 0.5, 1.5);
    ck_assert_double_gte(segments[1].enter.x, segments[0].exit.x);
    ck_assert_double_in_range(segments[1].enter.x, 0.5, 1.0);
    ck_assert_double_in_range(segments[1].exit.x, 1.5, 2.0);
    ck_assert_double_in_range(segments[1].length, 0.5, 1.5);

    cloudsGetLayerType().callback_delete(layer);
    rendererDelete(renderer);
}
END_TEST

START_TEST(test_clouds_preview_color)
{
    Renderer* renderer = cloudsCreatePreviewColorRenderer();

    /* TODO Test the density overriding */

    rendererDelete(renderer);
}
END_TEST

TEST_CASE(clouds, test_clouds_density, test_clouds_walking_boundaries, test_clouds_primary_segments, test_clouds_preview_color)
