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

TEST_CASE(terrain_painting, test_terrain_painting_grid)

