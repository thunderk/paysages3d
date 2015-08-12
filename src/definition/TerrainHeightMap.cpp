#include "TerrainHeightMap.h"

#include "TerrainDefinition.h"
#include "PaintedGridBrush.h"

TerrainHeightMap::TerrainHeightMap(TerrainDefinition* terrain):
    PaintedGrid(terrain), terrain(terrain)
{
}

void TerrainHeightMap::copy(DefinitionNode* _destination) const
{
    TerrainHeightMap* destination = (TerrainHeightMap*)_destination;

    destination->terrain = terrain;

    PaintedGrid::copy(destination);
}

double TerrainHeightMap::getInitialValue(double x, double y) const
{
    return terrain->getInterpolatedHeight(x * terrain->scaling, y * terrain->scaling, false, false);
}

void TerrainHeightMap::brushElevation(const PaintedGridBrush &brush, double x, double y, double value)
{
    PaintedGridBrushRaiseLower sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height);
}

void TerrainHeightMap::brushFlatten(const PaintedGridBrush &brush, double x, double y, double height, double force)
{
    PaintedGridBrushFlatten sbrush(brush, height);
    applyBrush(sbrush, x, y, force / terrain->height);
}

void TerrainHeightMap::brushSmooth(const PaintedGridBrush &brush, double x, double y, double value)
{
    PaintedGridBrushSmooth sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height);
}

void TerrainHeightMap::brushAddNoise(const PaintedGridBrush &brush, double x, double y, NoiseGenerator* generator, double value)
{
    PaintedGridBrushAddNoise sbrush(brush, generator);
    applyBrush(sbrush, x, y, value / terrain->height);
}

void TerrainHeightMap::brushReset(const PaintedGridBrush &brush, double x, double y, double value)
{
    PaintedGridBrushReset sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height);
}
