#ifndef TERRAINHEIGHTMAPBRUSH_H
#define TERRAINHEIGHTMAPBRUSH_H

#include "definition_global.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrush() {}
    TerrainHeightMapBrush(double relative_x, double relative_z, double hard_radius, double smoothed_size, double total_radius):
        relative_x(relative_x), relative_z(relative_z), hard_radius(hard_radius), smoothed_size(smoothed_size), total_radius(total_radius) {}

    void apply(TerrainHeightMap* heightmap, double force);

protected:
    virtual double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const;

public:
    double relative_x;
    double relative_z;
    double hard_radius;
    double smoothed_size;
    double total_radius;
};

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrushElevation: public TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrushElevation(const TerrainHeightMapBrush& brush) : TerrainHeightMapBrush(brush) {}
protected:
    double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const override;
};

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrushSmooth: public TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrushSmooth(const TerrainHeightMapBrush& brush) : TerrainHeightMapBrush(brush) {}
protected:
    double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const override;
};

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrushAddNoise: public TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrushAddNoise(NoiseGenerator* generator) : generator(generator) {}
    TerrainHeightMapBrushAddNoise(const TerrainHeightMapBrush& brush, NoiseGenerator* generator) : TerrainHeightMapBrush(brush), generator(generator) {}
protected:
    double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const override;
private:
    NoiseGenerator* generator;
};

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrushReset: public TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrushReset(const TerrainHeightMapBrush& brush) : TerrainHeightMapBrush(brush) {}
protected:
    double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const override;
};

class DEFINITIONSHARED_EXPORT TerrainHeightMapBrushFlatten: public TerrainHeightMapBrush
{
public:
    TerrainHeightMapBrushFlatten(double height) : height(height) {}
    TerrainHeightMapBrushFlatten(const TerrainHeightMapBrush& brush, double height) : TerrainHeightMapBrush(brush), height(height) {}
protected:
    double getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const override;
private:
    double height;
};

}
}

#endif // TERRAINHEIGHTMAPBRUSH_H
