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
    return terrain->getInterpolatedHeight(x, y, false, false);
}

void TerrainHeightMap::brushElevation(const PaintedGridBrush &brush, double x, double y, double value, bool commit)
{
    PaintedGridBrushRaiseLower sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height, commit);
}

void TerrainHeightMap::brushFlatten(const PaintedGridBrush &brush, double x, double y, double height, double force, bool commit)
{
    PaintedGridBrushFlatten sbrush(brush, height);
    applyBrush(sbrush, x, y, force / terrain->height, commit);
}

void TerrainHeightMap::brushSmooth(const PaintedGridBrush &brush, double x, double y, double value, bool commit)
{
    PaintedGridBrushSmooth sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height, commit);
}

void TerrainHeightMap::brushAddNoise(const PaintedGridBrush &brush, double x, double y, NoiseGenerator* generator, double value, bool commit)
{
    PaintedGridBrushAddNoise sbrush(brush, generator);
    applyBrush(sbrush, x, y, value / terrain->height, commit);
}

void TerrainHeightMap::brushReset(const PaintedGridBrush &brush, double x, double y, double value, bool commit)
{
    PaintedGridBrushReset sbrush(brush);
    applyBrush(sbrush, x, y, value / terrain->height, commit);
}

void TerrainHeightMap::clearPainting()
{
    PaintedGrid::clearPainting();

    terrain->validate();
}

void TerrainHeightMap::endBrushStroke()
{
    PaintedGrid::endBrushStroke();

    terrain->validate();
}
