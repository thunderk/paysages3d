#include "BaseTestCase.h"

#include <cmath>
#include "Maths.h"
#include "NoiseNode.h"
#include "TestToolNoise.h"
#include "TerrainDefinition.h"
#include "TerrainHeightMap.h"
#include "PaintedGridBrush.h"
#include "FloatNode.h"

// Noise sin period is defined at 20.0
static constexpr double X_FACTOR = Maths::PI / 10.0;

namespace {
class SinFractalNoise : public FractalNoise {
  public:
    SinFractalNoise() {
        setScaling(1.0, 2.0);
        setStep(0.0);
        NoiseState state;
        state.setLevelCount(1);
        state.setLevel(0, 0.0, 0.0, 0.0);
        setState(state);
    }
    virtual ~SinFractalNoise();
    virtual double getBase1d(double x) const override {
        return sin(x * X_FACTOR) * 0.5;
    }
    virtual double getBase2d(double x, double) const override {
        return sin(x * X_FACTOR) * 0.5;
    }
    virtual double getBase3d(double x, double, double) const override {
        return sin(x * X_FACTOR) * 0.5;
    }
};

SinFractalNoise::~SinFractalNoise() {
}

class TerrainPainting_Test : public BaseTestCase {
  public:
    virtual ~TerrainPainting_Test();

  protected:
    virtual void SetUp() {
        terrain = new TerrainDefinition(NULL);
        terrain->propHeightNoise()->forceSetGenerator(new SinFractalNoise);
        terrain->propWaterHeight()->setValue(0.0);
    }

    virtual void TearDown() {
        delete terrain;
    }

    TerrainDefinition *terrain;
    NoiseState noise_state;
};
TerrainPainting_Test::~TerrainPainting_Test() {
}
}

TEST_F(TerrainPainting_Test, grid) {
    /* Test base grid */
    EXPECT_DOUBLE_EQ(0.0, terrain->getGridHeight(0, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrain->getGridHeight(0, 1, 0));
    EXPECT_DOUBLE_EQ(0.0, terrain->getGridHeight(0, 0, 1));
    EXPECT_DOUBLE_EQ(sin(1.0 * X_FACTOR), terrain->getGridHeight(1, 0, 0));
    EXPECT_DOUBLE_EQ(sin(2.0 * X_FACTOR), terrain->getGridHeight(2, 0, 0));
    EXPECT_DOUBLE_EQ(sin(3.0 * X_FACTOR), terrain->getGridHeight(3, 0, 0));
    EXPECT_DOUBLE_EQ(sin(4.0 * X_FACTOR), terrain->getGridHeight(4, 0, 0));
    EXPECT_DOUBLE_EQ(1.0, terrain->getGridHeight(5, 0, 0));
    EXPECT_DOUBLE_EQ(sin(4.0 * X_FACTOR), terrain->getGridHeight(6, 0, 0));
    EXPECT_DOUBLE_EQ(-sin(1.0 * X_FACTOR), terrain->getGridHeight(-1, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrain->getGridHeight(10, 0, 0));
    EXPECT_DOUBLE_EQ(-1.0, terrain->getGridHeight(15, 0, 0));
    EXPECT_DOUBLE_EQ(0.0, terrain->getGridHeight(20, 0, 0));
    EXPECT_DOUBLE_EQ(-1.0, terrain->getGridHeight(-5, 0, 0));

    /* Test interpolated result */
    EXPECT_DOUBLE_EQ(terrain->getInterpolatedHeight(0.0, 0.0, false, false), 0.0);
    EXPECT_DOUBLE_IN_RANGE(terrain->getInterpolatedHeight(0.5, 0.0, false, false), 0.1564, 0.1566);
    EXPECT_DOUBLE_EQ(terrain->getInterpolatedHeight(1.0, 0.0, false, false), sin(X_FACTOR));
}

static void _checkBrushResultSides(TerrainDefinition *terrain, PaintedGridBrush *, double center, double midhard,
                                   double hard, double midsoft, double soft, double exter, double neg_midhard,
                                   double neg_hard, double neg_midsoft, double neg_soft, double neg_exter) {
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(0, 0, 1), center);

    EXPECT_DOUBLE_EQ(terrain->getGridHeight(1, 0, 1), midhard);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(2, 0, 1), hard);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(3, 0, 1), midsoft);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(4, 0, 1), soft);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(5, 0, 1), exter);

    EXPECT_DOUBLE_EQ(terrain->getGridHeight(-1, 0, 1), neg_midhard);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(-2, 0, 1), neg_hard);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(-3, 0, 1), neg_midsoft);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(-4, 0, 1), neg_soft);
    EXPECT_DOUBLE_EQ(terrain->getGridHeight(-5, 0, 1), neg_exter);
}

static void _checkBrushResult(TerrainDefinition *terrain, PaintedGridBrush *brush, double center, double midhard,
                              double hard, double midsoft, double soft, double exter, int mirror) {
    if (mirror) {
        _checkBrushResultSides(terrain, brush, center, midhard, hard, midsoft, soft, exter, -midhard, -hard, -midsoft,
                               -soft, -exter);
    } else {
        _checkBrushResultSides(terrain, brush, center, midhard, hard, midsoft, soft, exter, midhard, hard, midsoft,
                               soft, exter);
    }
}

TEST_F(TerrainPainting_Test, brush_flatten) {
    /* Set up */
    PaintedGridBrush brush(2.0, 2.0, 4.0);
    terrain->propHeightNoise()->forceSetGenerator(new ConstantFractalNoise(0.0));

    /* Test flattening center at 0.5 */
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrain->height_map->brushFlatten(brush, 0.0, 0.0, 0.5, 1.0, true);
    _checkBrushResult(terrain, &brush, 0.5, 0.5, 0.5, 0.25, 0.0, 0.0, 0);

    /* Test brush strength */
    terrain->height_map->clearPainting();
    _checkBrushResult(terrain, &brush, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
    terrain->height_map->brushFlatten(brush, 0.0, 0.0, 0.5, 0.01, true);
    _checkBrushResult(terrain, &brush, 0.005, 0.005, 0.005, 0.0025, 0.0, 0.0, 0);

    /* Test cumulative effect */
    terrain->height_map->brushFlatten(brush, 0.0, 0.0, 0.5, 0.01, true);
    _checkBrushResult(terrain, &brush, 0.00995, 0.00995, 0.00995, 0.0049875, 0.0, 0.0, 0);
}

TEST_F(TerrainPainting_Test, brush_reset) {
    /* Set up */
    PaintedGridBrush brush(2.0, 2.0, 4.0);
    PaintedGridBrush brush_full(4.0, 0.0, 4.0);
    terrain->propHeightNoise()->forceSetGenerator(new ConstantFractalNoise(1.0));

    /* Test resetting at center */
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrain->height_map->brushFlatten(brush_full, 0.0, 0.0, 2.0, 1.0, true);
    _checkBrushResult(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 0);
    terrain->height_map->brushReset(brush, 0.0, 0.0, 1.0, true);
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.5, 2.0, 1.0, 0);

    /* Test brush strength */
    terrain->height_map->clearPainting();
    _checkBrushResult(terrain, &brush, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0);
    terrain->height_map->brushFlatten(brush_full, 0.0, 0.0, 2.0, 1.0, true);
    _checkBrushResult(terrain, &brush, 2.0, 2.0, 2.0, 2.0, 2.0, 1.0, 0);
    terrain->height_map->brushReset(brush, 0.0, 0.0, 0.1, true);
    _checkBrushResult(terrain, &brush, 1.9, 1.9, 1.9, 1.95, 2.0, 1.0, 0);

    /* Test cumulative effect */
    terrain->height_map->brushReset(brush, 0.0, 0.0, 0.1, true);
    _checkBrushResult(terrain, &brush, 1.81, 1.81, 1.81, 1.9025, 2.0, 1.0, 0);
}
