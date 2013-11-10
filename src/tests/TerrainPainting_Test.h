#ifndef TERRAINPAINTING_TEST_H
#define TERRAINPAINTING_TEST_H

#include "BaseTestCase.h"

class TerrainPainting_Test: public BaseTestCase
{
public:
    CPPUNIT_TEST_SUITE(TerrainPainting_Test);
    CPPUNIT_TEST(test_terrain_painting_grid);
    CPPUNIT_TEST(test_terrain_painting_brush_flatten);
    CPPUNIT_TEST(test_terrain_painting_brush_reset);
    CPPUNIT_TEST_SUITE_END();

    void test_terrain_painting_grid();
    void test_terrain_painting_brush_flatten();
    void test_terrain_painting_brush_reset();
};

#endif // TERRAINPAINTING_TEST_H
