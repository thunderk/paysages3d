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

static void _checkBrushResultSides(TerrainDefinition* terrain, TerrainBrush* brush, double center, double midhard, double hard, double midsoft, double soft, double exter, double neg_midhard, double neg_hard, double neg_midsoft, double neg_soft, double neg_exter)
{
    UNUSED(brush);

    ck_assert_double_eq(terrainGetGridHeight(terrain, 0, 0, 1), center);

    ck_assert_double_eq(terrainGetGridHeight(terrain, 1, 0, 1), midhard);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 2, 0, 1), hard);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 3, 0, 1), midsoft);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 4, 0, 1), soft);
    ck_assert_double_eq(terrainGetGridHeight(terrain, 5, 0, 1), exter);

    ck_assert_double_eq(terrainGetGridHeight(terrain, -1, 0, 1), neg_midhard);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -2, 0, 1), neg_hard);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -3, 0, 1), neg_midsoft);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -4, 0, 1), neg_soft);
    ck_assert_double_eq(terrainGetGridHeight(terrain, -5, 0, 1), neg_exter);
}


static void _checkBrushResult(TerrainDefinition* terrain, TerrainBrush* brush, double center, double midhard, double hard, double midsoft, double soft, double exter, int mirror)
{
    if (mirror)
    {
        _checkBrushResultSides(terrain, brush, center, midhard, hard, midsoft, soft, exter, -midhard, -hard, -midsoft, -soft, -exter);
    }
    else
    {
        _checkBrushResultSides(terrain, brush, center, midhard, hard, midsoft, soft, exter, midhard, hard, midsoft, soft, exter);
    }
}

START_TEST(test_terrain_painting_brush_flatten)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();
    TerrainBrush brush = {0.0, 0.0, 2.0, 2.0, 4.0};
    terrain->height = 1.0;
    terrain->scaling = 1.0;
    noiseForceValue(terrain->_height_noise, 0.0);

    /* Test flattening center at 0.5 */
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    _checkBrushResult(terrain, &brush, 0.5, 0.5, 0.5, 0.25, 0.0, 0.0, 0);

    /* Test brush strength */
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 0.01);
    _checkBrushResult(terrain, &brush, 0.005, 0.005, 0.005, 0.0025, 0.0, 0.0, 0);

    /* Test cumulative effect */
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 0.01);
    _checkBrushResult(terrain, &brush, 0.00995, 0.00995, 0.00995, 0.0049875, 0.0, 0.0, 0);

    /* Test with height modifier */
    terrain->height = 10.0;
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    _checkBrushResult(terrain, &brush, 0.05, 0.05, 0.05, 0.025, 0.0, 0.0, 0);

    /* Test with scaling modifier */
    terrain->height = 10.0;
    terrain->scaling = 2.0;
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush, 0.5, 1.0);
    _checkBrushResult(terrain, &brush, 0.05, 0.05, 0.05, 0.025, 0.0, 0.0, 0);

    /* Tear down */
    _tearDownDefinition(terrain);
}

END_TEST

START_TEST(test_terrain_painting_brush_reset)
{
    /* Set up */
    TerrainDefinition* terrain = _setUpDefinition();
    TerrainBrush brush = {0.0, 0.0, 2.0, 2.0, 4.0};
    TerrainBrush brush_full = {0.0, 0.0, 4.0, 0.0, 4.0};
    terrain->height = 1.0;
    terrain->scaling = 1.0;
    noiseForceValue(terrain->_height_noise, 1.0);

    /* Test resetting at center */
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResult(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 0);
    terrainBrushReset(terrain->height_map, &brush, 1.0);
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.5, 2.0, 1.0, 0);

    /* Test brush strength */
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResult(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 0);
    terrainBrushReset(terrain->height_map, &brush, 0.1);
    _checkBrushResult(terrain, &brush, 1.9, 1.9, 1.9, 1.95, 2.0, 1.0, 0);

    /* Test cumulative effect */
    terrainBrushReset(terrain->height_map, &brush, 0.1);
    _checkBrushResult(terrain, &brush, 1.81, 1.81, 1.81, 1.9025, 2.0, 1.0, 0);

    /* Test with height modifier */
    terrain->height = 10.0;
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResult(terrain, &brush, 1.1, 1.1, 1.1, 1.1, 1.1, 1.0, 0);
    terrainBrushReset(terrain->height_map, &brush, 0.1);
    _checkBrushResult(terrain, &brush, 1.099, 1.099, 1.099, 1.0995, 1.1, 1.0, 0);

    /* Test with scaling modifier */
    terrain->height = 10.0;
    terrain->scaling = 2.0;
    terrainClearPainting(terrain->height_map);
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResult(terrain, &brush, 1.1, 1.1, 1.1, 1.1, 1.1, 1.0, 0);
    terrainBrushReset(terrain->height_map, &brush, 0.1);
    _checkBrushResult(terrain, &brush, 1.099, 1.099, 1.099, 1.0995, 1.1, 1.0, 0);

    /* Tear down */
    _tearDownDefinition(terrain);

    /* Test with scaling and the sinusoid setup (to test the basevalue sampling) */
    terrain = _setUpDefinition();
    terrain->height = 1.0;
    terrain->scaling = 2.0;
    _checkBrushResult(terrain, &brush, 0.0, 0.309016994375, 0.587785252292, 0.809016994375, 0.951056516295, 1.0, 1);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResultSides(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 2.0, 2.0, 2.0, 2.0, -1.0);
    terrainBrushReset(terrain->height_map, &brush, 1.0);
    _checkBrushResultSides(terrain, &brush, 0.0, 0.309016994375, 0.587785252292, 2.0 - (2.0 - 0.809016994375) * 0.5, 2.0, 1.0, -0.309016994375, -0.587785252292, 2.0 - (2.0 + 0.809016994375) * 0.5, 2.0, -1.0);
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

