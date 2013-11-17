#include "TerrainHeightMapBrush.h"

#include "TerrainDefinition.h"
#include "TerrainHeightMap.h"
#include "NoiseGenerator.h"

typedef struct
{
    int xstart;
    int xend;
    int xsize;
    int zstart;
    int zend;
    int zsize;
} IntegerRect;

static inline IntegerRect _getBrushRect(TerrainHeightMapBrush* brush)
{
    IntegerRect result;
    double s = brush->smoothed_size + brush->hard_radius;

    result.xstart = (int)floor(brush->relative_x - s);
    result.xend = (int)ceil(brush->relative_x + s);
    result.zstart = (int)floor(brush->relative_z - s);
    result.zend = (int)ceil(brush->relative_z + s);

    result.xsize = result.xend - result.xstart + 1;
    result.zsize = result.zend - result.zstart + 1;

    return result;
}

static inline int _isInRect(const IntegerRect &rect, int x, int z)
{
    return (x >= rect.xstart && x <= rect.xend && z >= rect.zstart && z <= rect.zend);
}

void TerrainHeightMapBrush::apply(TerrainHeightMap* heightmap, double force)
{
    IntegerRect brush_rect = _getBrushRect(this);
    int x, z;
    double dx, dz, distance, influence;

    force /= heightmap->terrain->height;

    for (x = brush_rect.xstart; x <= brush_rect.xend; x++)
    {
        dx = (double)x;
        for (z = brush_rect.zstart; z <= brush_rect.zend; z++)
        {
            dz = (double)z;
            distance = sqrt((relative_x - dx) * (relative_x - dx) + (relative_z - dz) * (relative_z - dz));

            if (distance > hard_radius)
            {
                if (distance <= hard_radius + smoothed_size)
                {
                    influence = (1.0 - (distance - hard_radius) / smoothed_size);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                influence = 1.0;
            }

            double* dpointer = heightmap->getDataPointer(heightmap->brush_data, x, z, heightmap->merged_data, heightmap->terrain, 1);
            *dpointer = getBrushValue(heightmap, dx, dz, *dpointer, influence, force);
        }
    }
}

double TerrainHeightMapBrush::getBrushValue(TerrainHeightMap*, double, double, double basevalue, double, double) const
{
    return basevalue;
}

double TerrainHeightMapBrushElevation::getBrushValue(TerrainHeightMap*, double, double, double basevalue, double influence, double force) const
{
    return basevalue + influence * force;
}

double TerrainHeightMapBrushSmooth::getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const
{
    TerrainDefinition* terrain = heightmap->terrain;
    double ideal, factor;
    ideal = terrain->getInterpolatedHeight((x + total_radius * 0.5) * terrain->scaling, z * terrain->scaling, 0, 1);
    ideal += terrain->getInterpolatedHeight((x - total_radius * 0.5) * terrain->scaling, z * terrain->scaling, 0, 1);
    ideal += terrain->getInterpolatedHeight(x * terrain->scaling, (z - total_radius * 0.5) * terrain->scaling, 0, 1);
    ideal += terrain->getInterpolatedHeight(x * terrain->scaling, (z + total_radius * 0.5) * terrain->scaling, 0, 1);
    ideal /= 4.0;
    factor = influence * force;
    if (factor > 1.0)
    {
        factor = 0.0;
    }
    return basevalue + (ideal - basevalue) * factor;
}

double TerrainHeightMapBrushAddNoise::getBrushValue(TerrainHeightMap*, double x, double z, double basevalue, double influence, double force) const
{
    return basevalue + generator->get2DTotal(x / total_radius, z / total_radius) * influence * force * total_radius;
}

double TerrainHeightMapBrushReset::getBrushValue(TerrainHeightMap* heightmap, double x, double z, double basevalue, double influence, double force) const
{
    TerrainDefinition* terrain = heightmap->terrain;
    double ideal = terrain->getInterpolatedHeight(x * terrain->scaling, z * terrain->scaling, 0, 0);
    return basevalue + (ideal - basevalue) * influence * force;
}

double TerrainHeightMapBrushFlatten::getBrushValue(TerrainHeightMap*, double, double, double basevalue, double influence, double force) const
{
    double ideal = height;
    return basevalue + (ideal - basevalue) * influence * force;
}
