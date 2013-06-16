#include "testing/common.h"

#include <math.h>
#include "rendering/tools.h"
#include "rendering/terrain/public.h"

/* Noise sin period is defined at 20.0 */
#define X_FACTOR (M_PI / 10.0)

static double _noise1dMock(double x)
{
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

static double _noise2dMock(double x, double y)
{
    UNUSED(y);
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

static double _noise3dMock(double x, double y, double z)
{
    UNUSED(y);
    UNUSED(z);
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

static TerrainDefinition* _setUpDefinition()
{
    TerrainDefinition* terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
    terrain->height = 3.0;
    terrain->scaling = 1.0;
    noiseClearLevels(terrain->_height_noise);
    NoiseLevel level = {1.0, 2.0, -1.0, 0.0, 0.0, 0.0};
    noiseAddLevel(terrain->_height_noise, level, 0);
    noiseSetCustomFunction(terrain->_height_noise, _noise1dMock, _noise2dMock, _noise3dMock);
    return terrain;
}

static void _tearDownDefinition(TerrainDefinition* terrain)
{
    TerrainDefinitionClass.destroy(terrain);
}

START_TEST(test_terrain_painting_grid)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();

    /* Test base grid */
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 0), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 1, 0), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 0), sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 0), sin(2.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 0), sin(3.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 0), sin(4.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 0), 1.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 6, 0, 0), sin(4.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 0), -sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 10, 0, 0), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 15, 0, 0), -1.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 20, 0, 0), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -5, 0, 0), -1.0);

    /* Test interpolated result */
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 0.0, 0.0, 0, 0), 0.0);
    ck_assert_double_in_range(terrainGetInterpolatedHeight(terrain, 0.5, 0.0, 0, 0), 0.1564, 0.1566);
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 1.0, 0.0, 0, 0), sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 0.0, 0.0, 1, 0), 0.0);
    ck_assert_double_in_range(terrainGetInterpolatedHeight(terrain, 0.5, 0.0, 1, 0), 3.0 * 0.1564, 3.0 * 0.1566);
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 1.0, 0.0, 1, 0), 3.0 * sin(1.0 * X_FACTOR));

    /* Test scaling */
    terrain->scaling = 2.0;
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 0), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 0), sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 0), sin(2.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 0), sin(3.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 0, 0, 0, 0), 0.0);
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 1, 0, 0, 0), sin(0.5 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 2, 0, 0, 0), sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 3, 0, 0, 0), sin(1.5 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 0, 0, 1, 0), 0.0);
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 1, 0, 1, 0), 6.0 * sin(0.5 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 2, 0, 1, 0), 6.0 * sin(1.0 * X_FACTOR));
    ck_assert_double_eq(terrainGetInterpolatedHeight(terrain, 3, 0, 1, 0), 6.0 * sin(1.5 * X_FACTOR));

    /* Tear down */
    _tearDownDefinition(terrain);
}
END_TEST

START_TEST(test_terrain_painting_brush_elevation)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();

    /* Test */
    /* TODO */

    /* Tear down */
    _tearDownDefinition(terrain);
}
END_TEST

START_TEST(test_terrain_painting_brush_noise)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();

    /* Test */
    /* TODO */

    /* Tear down */
    _tearDownDefinition(terrain);
}

END_TEST

START_TEST(test_terrain_painting_brush_smooth)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();

    /* Test */
    /* TODO */

    /* Tear down */
    _tearDownDefinition(terrain);
}

END_TEST

START_TEST(test_terrain_painting_brush_flatten)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();
    TerrainBrush brush;
    terrain->height = 1.0;
    terrain->scaling = 1.0;
    noiseForceValue(terrain->_height_noise, 0.0);

    /* Test flattening center at 0.5 */
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);
    brush.relative_x = 0.0;
    brush.relative_z = 0.0;
    brush.hard_radius = 2.0;
    brush.smoothed_size = 2.0;
    brush.total_radius = 4.0;
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.5);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.5);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.5);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.5);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.25);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);

    /* Test brush strength */
    terrainClearPainting(terrain->height_map);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);
    brush.relative_x = 0.0;
    brush.relative_z = 0.0;
    brush.hard_radius = 2.0;
    brush.smoothed_size = 2.0;
    brush.total_radius = 4.0;
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 0.01);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.005);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.005);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.005);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.005);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0025);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);

    /* Test cumulative effect */
    brush.relative_x = 0.0;
    brush.relative_z = 0.0;
    brush.hard_radius = 2.0;
    brush.smoothed_size = 2.0;
    brush.total_radius = 4.0;
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 0.01);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.00995);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.00995);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.00995);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.00995);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0049875);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);

    /* Test with height modifier */
    terrain->height = 10.0;
    terrainClearPainting(terrain->height_map);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);
    brush.relative_x = 0.0;
    brush.relative_z = 0.0;
    brush.hard_radius = 2.0;
    brush.smoothed_size = 2.0;
    brush.total_radius = 4.0;
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.025);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);

    /* Test with scaling modifier */
    terrain->height = 10.0;
    terrain->scaling = 2.0;
    terrainClearPainting(terrain->height_map);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);
    brush.relative_x = 0.0;
    brush.relative_z = 0.0;
    brush.hard_radius = 2.0;
    brush.smoothed_size = 2.0;
    brush.total_radius = 4.0;
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), 0.05);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), 0.025);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), 0.0);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), 0.0);

    /* Tear down */
    _tearDownDefinition(terrain);
}

END_TEST

START_TEST(test_terrain_painting_brush_reset)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();

    /* Test */
    /* TODO */

    /* Tear down */
    _tearDownDefinition(terrain);
}
END_TEST

TEST_CASE(terrain_painting,
          test_terrain_painting_grid,
          test_terrain_painting_brush_elevation,
          test_terrain_painting_brush_noise,
          test_terrain_painting_brush_smooth,
          test_terrain_painting_brush_flatten,
          test_terrain_painting_brush_reset)

