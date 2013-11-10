#include "BaseTestCase.h"

#include <cmath>
#include "NoiseGenerator.h"
#include "terrain/public.h"

/* Noise sin period is defined at 20.0 */
#define X_FACTOR (M_PI / 10.0)

static double _noise1dMock(double x)
{
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

static double _noise2dMock(double x, double)
{
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

static double _noise3dMock(double x, double, double)
{
    return sin(x * X_FACTOR) * 0.5 + 0.5;
}

class TerrainPainting_Test : public BaseTestCase {
protected:
    virtual void SetUp() {
        terrain = (TerrainDefinition*)TerrainDefinitionClass.create();
        terrain->height = 3.0;
        terrain->scaling = 1.0;
        terrain->_height_noise->clearLevels();
        NoiseLevel level = {1.0, 2.0, -1.0, 0.0, 0.0, 0.0};
        terrain->_height_noise->addLevel(level, 0);
        terrain->_height_noise->setCustomFunction(_noise1dMock, _noise2dMock, _noise3dMock);
    }

    virtual void TearDown()
    {
        TerrainDefinitionClass.destroy(terrain);
    }

    TerrainDefinition* terrain;
};

TEST_F(TerrainPainting_Test, grid)
{
    /* Test base grid */
    EXPECT_DOUBLE_EQ(0.0, terrainGetGridHeight(terrain, 0, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrainGetGridHeight(terrain, 0, 1, 0));
    EXPECT_DOUBLE_EQ(0.0, terrainGetGridHeight(terrain, 0, 0, 1));
    EXPECT_DOUBLE_EQ(sin(1.0 * X_FACTOR), terrainGetGridHeight(terrain, 1, 0, 0));
    EXPECT_DOUBLE_EQ(sin(2.0 * X_FACTOR), terrainGetGridHeight(terrain, 2, 0, 0));
    EXPECT_DOUBLE_EQ(sin(3.0 * X_FACTOR), terrainGetGridHeight(terrain, 3, 0, 0));
    EXPECT_DOUBLE_EQ(sin(4.0 * X_FACTOR), terrainGetGridHeight(terrain, 4, 0, 0));
    EXPECT_DOUBLE_EQ(1.0, terrainGetGridHeight(terrain, 5, 0, 0));
    EXPECT_DOUBLE_EQ(sin(4.0 * X_FACTOR), terrainGetGridHeight(terrain, 6, 0, 0));
    EXPECT_DOUBLE_EQ(-sin(1.0 * X_FACTOR), terrainGetGridHeight(terrain, -1, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrainGetGridHeight(terrain, 10, 0, 0));
    EXPECT_DOUBLE_EQ(-1.0, terrainGetGridHeight(terrain, 15, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrainGetGridHeight(terrain, 20, 0, 0));
    EXPECT_DOUBLE_EQ(-1.0, terrainGetGridHeight(terrain, -5, 0, 0));

    /* Test interpolated result */
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 0.0, 0.0, 0, 0), 0.0);
    EXPECT_DOUBLE_IN_RANGE(terrainGetInterpolatedHeight(terrain, 0.5, 0.0, 0, 0), 0.1564, 0.1566);
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 1.0, 0.0, 0, 0), sin(1.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 0.0, 0.0, 1, 0), 0.0);
    EXPECT_DOUBLE_IN_RANGE(terrainGetInterpolatedHeight(terrain, 0.5, 0.0, 1, 0), 3.0 * 0.1564, 3.0 * 0.1566);
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 1.0, 0.0, 1, 0), 3.0 * sin(1.0 * X_FACTOR));

    /* Test scaling */
    terrain->scaling = 2.0;
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 0, 0, 0), 0.0);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 1, 0, 0), sin(1.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 2, 0, 0), sin(2.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 3, 0, 0), sin(3.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 0, 0, 0, 0), 0.0);
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 1, 0, 0, 0), sin(0.5 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 2, 0, 0, 0), sin(1.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 3, 0, 0, 0), sin(1.5 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 0, 0, 1, 0), 0.0);
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 1, 0, 1, 0), 6.0 * sin(0.5 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 2, 0, 1, 0), 6.0 * sin(1.0 * X_FACTOR));
    EXPECT_DOUBLE_EQ(terrainGetInterpolatedHeight(terrain, 3, 0, 1, 0), 6.0 * sin(1.5 * X_FACTOR));
}

static void _checkBrushResultSides(TerrainDefinition* terrain, TerrainBrush*, double center, double midhard, double hard, double midsoft, double soft, double exter, double neg_midhard, double neg_hard, double neg_midsoft, double neg_soft, double neg_exter)
{
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 0, 0, 1), center);

    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 1, 0, 1), midhard);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 2, 0, 1), hard);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 3, 0, 1), midsoft);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 4, 0, 1), soft);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, 5, 0, 1), exter);

    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, -1, 0, 1), neg_midhard);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, -2, 0, 1), neg_hard);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, -3, 0, 1), neg_midsoft);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, -4, 0, 1), neg_soft);
    EXPECT_DOUBLE_EQ(terrainGetGridHeight(terrain, -5, 0, 1), neg_exter);
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

TEST_F(TerrainPainting_Test, brush_flatten)
{
    /* Set up */
    TerrainBrush brush = {0.0, 0.0, 2.0, 2.0, 4.0};
    terrain->height = 1.0;
    terrain->scaling = 1.0;
    terrain->_height_noise->forceValue(0.0);

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
}

TEST_F(TerrainPainting_Test, brush_reset)
{
    /* Set up */
    TerrainBrush brush = {0.0, 0.0, 2.0, 2.0, 4.0};
    TerrainBrush brush_full = {0.0, 0.0, 4.0, 0.0, 4.0};
    terrain->height = 1.0;
    terrain->scaling = 1.0;
    terrain->_height_noise->forceValue(1.0);

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
}

TEST_F(TerrainPainting_Test, brush_reset_basevalue)
{
    /* Set up */
    TerrainBrush brush = {0.0, 0.0, 2.0, 2.0, 4.0};
    TerrainBrush brush_full = {0.0, 0.0, 4.0, 0.0, 4.0};
    terrain->height = 1.0;
    terrain->scaling = 1.0;

    /* Test with scaling and the sinusoid setup (to test the basevalue sampling) */
    terrain->height = 1.0;
    terrain->scaling = 2.0;
    _checkBrushResult(terrain, &brush, 0.0, 0.309016994375, 0.587785252292, 0.809016994375, 0.951056516295, 1.0, 1);
    terrainBrushFlatten(terrain->height_map, &brush_full, 2.0, 1.0);
    _checkBrushResultSides(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 2.0, 2.0, 2.0, 2.0, -1.0);
    terrainBrushReset(terrain->height_map, &brush, 1.0);
    _checkBrushResultSides(terrain, &brush, 0.0, 0.309016994375, 0.587785252292, 2.0 - (2.0 - 0.809016994375) * 0.5, 2.0, 1.0, -0.309016994375, -0.587785252292, 2.0 - (2.0 + 0.809016994375) * 0.5, 2.0, -1.0);
}
