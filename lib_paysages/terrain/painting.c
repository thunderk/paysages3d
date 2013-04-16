#include "private.h"

/*
 * Terrain height map painting.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../tools/memory.h"
#include "../tools.h"
#include "../tools/array.h"

typedef struct
{
    int xstart;
    int xend;
    double* height;
} HeightMapPixelGroup;

typedef struct
{
    int z;
    int memsize;
    int pixel_groups_count;
    HeightMapPixelGroup* pixel_groups;
} HeightMapRow;

typedef struct
{
    int memsize;
    int rows_count;
    HeightMapRow* rows;
} HeightMapData;

struct TerrainHeightMap
{
    TerrainDefinition* terrain;
    HeightMapData merged_data;
    HeightMapData brush_data;
};

static void _initData(HeightMapData* data)
{
    data->rows_count = 0;
    data->rows = malloc(1);
}

static void _clearData(HeightMapData* data)
{
    int i, j;
    for (i = 0; i < data->rows_count; i++)
    {
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            free(data->rows[i].pixel_groups[j].height);
        }
        free(data->rows[i].pixel_groups);
    }
    data->rows_count = 0;
    data->rows = realloc(data->rows, 1);
}

static void _deleteData(HeightMapData* data)
{
    _clearData(data);
    free(data->rows);
}

static void _copyData(HeightMapData* source, HeightMapData* destination)
{
    _clearData(destination);

    /* TODO */
}

static void _saveData(PackStream* stream, HeightMapData* data)
{
    int i, j, k;
    packWriteInt(stream, &data->rows_count);
    for (i = 0; i < data->rows_count; i++)
    {
        packWriteInt(stream, &data->rows[i].z);
        packWriteInt(stream, &data->rows[i].pixel_groups_count);
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            packWriteInt(stream, &data->rows[i].pixel_groups[j].xstart);
            packWriteInt(stream, &data->rows[i].pixel_groups[j].xend);
            for (k = 0; k < data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart; k++)
            {
                packWriteDouble(stream, &data->rows[i].pixel_groups[j].height[k]);
            }
        }
    }
}

static void _loadData(PackStream* stream, HeightMapData* data)
{
    int i, j, k, n;

    _clearData(data);

    packReadInt(stream, &data->rows_count);
    data->rows = realloc(data->rows, sizeof(HeightMapRow) * data->rows_count);
    for (i = 0; i < data->rows_count; i++)
    {
        packReadInt(stream, &data->rows[i].z);
        packReadInt(stream, &data->rows[i].pixel_groups_count);
        data->rows[i].pixel_groups = malloc(sizeof(HeightMapPixelGroup) * data->rows[i].pixel_groups_count);
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            packReadInt(stream, &data->rows[i].pixel_groups[j].xstart);
            packReadInt(stream, &data->rows[i].pixel_groups[j].xend);
            n = data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart;
            data->rows[i].pixel_groups[j].height = malloc(sizeof(double) * n);
            for (k = 0; k < n; k++)
            {
                packReadDouble(stream, &data->rows[i].pixel_groups[j].height[k]);
            }
        }
    }
}

/*
 * Get a pointer to the data in a heightmap, to a certain location.
 * If the location is not already in the heightmap, it is initialized with the terrain height.
 * This method will grow the heightmap as necessary (if 'grow' is set to false, NULL will be returned on missing pixels).
 */
static double* _getDataPointer(HeightMapData* data, int x, int z, TerrainDefinition* terrain, int grow)
{
    int i;

    /* Find row */
    /* TODO Dichotomic search */
    HeightMapRow* row;
    i = 0;
    while (i < data->rows_count && data->rows[i].z < z)
    {
        i++;
    }
    if (i < data->rows_count && data->rows[i].z == z)
    {
        row = data->rows + i;
    }
    else if (grow)
    {
        row = naiveArrayInsert((void**)&data->rows, sizeof(HeightMapRow), data->rows_count, i);

        row->z = z;
        row->pixel_groups_count = 0;
        row->pixel_groups = malloc(1);

        data->rows_count++;
    }
    else
    {
        return NULL;
    }

    /* Find pixel group */
    HeightMapPixelGroup* pixel_group = NULL;
    for (i = 0; i < row->pixel_groups_count; i++)
    {
        if (x < row->pixel_groups[i].xstart - 1)
        {
            break;
        }
        if (x >= row->pixel_groups[i].xstart - 1 && x <= row->pixel_groups[i].xend + 1)
        {
            pixel_group = row->pixel_groups + i;
        }
    }

    /* Alter pixel group */
    double* pixel;
    int added = 1;
    if (!pixel_group)
    {
        if (!grow)
        {
            return NULL;
        }

        /* Create the pixel group with one pixel */
        pixel_group = naiveArrayInsert((void**)&row->pixel_groups, sizeof(HeightMapPixelGroup), row->pixel_groups_count, i);

        pixel_group->xstart = x;
        pixel_group->xend = x;
        pixel_group->height = malloc(sizeof(double));

        pixel = pixel_group->height;

        row->pixel_groups_count++;
    }
    else if (x == pixel_group->xstart - 1)
    {
        if (!grow)
        {
            return NULL;
        }

        /* Extend the rowgroup at start */
        pixel_group->xstart--;
        pixel = naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, 0);
    }
    else if (x == pixel_group->xend + 1)
    {
        if (!grow)
        {
            return NULL;
        }

        /* Extend the rowgroup at end */
        pixel_group->xend++;
        pixel = naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, pixel_group->xend - pixel_group->xstart);
    }
    else
    {
        assert(x >= pixel_group->xstart);
        assert(x <= pixel_group->xend);
        pixel = pixel_group->height + x - pixel_group->xstart;
        added = 0;
    }

    /* Reset pixel if it had been added */
    if (added && terrain)
    {
        *pixel = terrainGetGridHeight(terrain, x, z, 0);
    }
    return pixel;
}

TerrainHeightMap* terrainHeightMapCreate(TerrainDefinition* terrain)
{
    TerrainHeightMap* result;

    result = malloc(sizeof(TerrainHeightMap));
    result->terrain = terrain;
    _initData(&result->merged_data);
    _initData(&result->brush_data);

    return result;
}

void terrainHeightmapDelete(TerrainHeightMap* heightmap)
{
    _deleteData(&heightmap->merged_data);
    _deleteData(&heightmap->brush_data);
    free(heightmap);
}

void terrainHeightmapCopy(TerrainHeightMap* source, TerrainHeightMap* destination)
{
    destination->terrain = source->terrain;

    _copyData(&source->merged_data, &destination->merged_data);
    _clearData(&destination->brush_data);
}

void terrainHeightmapSave(PackStream* stream, TerrainHeightMap* heightmap)
{
    _saveData(stream, &heightmap->merged_data);
}

void terrainHeightmapLoad(PackStream* stream, TerrainHeightMap* heightmap)
{
    _loadData(stream, &heightmap->merged_data);
    _clearData(&heightmap->brush_data);
}

static inline int _getInterpolatedValue(HeightMapData* data, double x, double z, double* result, TerrainDefinition* terrain)
{
    /*if ((int)floor(x) >= chunk->rect.xstart && (int)floor(x) <= chunk->rect.xend && (int)floor(z) >= (double)chunk->rect.zstart && (int)floor(z) <= (double)chunk->rect.zend)
    {
        double stencil[16];
        int ix, iz, cx, cz;
        int xmax = chunk->rect.xsize - 1;
        int zmax = chunk->rect.zsize - 1;
        int xlow;
        int zlow;

        x -= chunk->rect.xstart;
        z -= chunk->rect.zstart;

        xlow = floor(x);
        zlow = floor(z);

        for (ix = xlow - 1; ix <= xlow + 2; ix++)
        {
            for (iz = zlow - 1; iz <= zlow + 2; iz++)
            {
                cx = ix < 0 ? 0 : ix;
                cx = cx > xmax ? xmax : cx;
                cz = iz < 0 ? 0 : iz;
                cz = cz > zmax ? zmax : cz;
                stencil[(iz - (zlow - 1)) * 4 + ix - (xlow - 1)] = chunk->data[cz * chunk->rect.xsize + cx];
            }
        }

        if (result)
        {
            *result = toolsBicubicInterpolate(stencil, x - (double)xlow, z - (double)zlow);
        }

        return 1;
    }
    else
    {
        return 0;
    }*/
    return 0;
}

int terrainHeightmapGetHeight(TerrainHeightMap* heightmap, double x, double z, double* result)
{
    /* TODO */
    return 0;
}

static inline IntegerRect _getBrushRect(TerrainBrush* brush)
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

static inline int _isInRect(IntegerRect rect, int x, int z)
{
    return (x >= rect.xstart && x <= rect.xend && z >= rect.zstart && z <= rect.zend);
}

size_t terrainGetMemoryStats(TerrainDefinition* definition)
{
    TerrainHeightMap* heightmap = definition->height_map;
    size_t result = 0;
    int i;

    /*if (heightmap->floating_used)
    {
        result += sizeof(double) * heightmap->floating_data.rect.xsize * heightmap->floating_data.rect.zsize;
    }
    for (i = 0; i < heightmap->fixed_count; i++)
    {
        result += sizeof(double) * heightmap->fixed_data[i].rect.xsize * heightmap->fixed_data[i].rect.zsize;
    }*/

    return result;
}

int terrainIsPainted(TerrainHeightMap* heightmap, int x, int z)
{
    return _getDataPointer(&heightmap->brush_data, x, z, NULL, 0) || _getDataPointer(&heightmap->merged_data, x, z, NULL, 0);
}

typedef double (*BrushCallback)(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data);

static inline void _applyBrush(TerrainHeightMap* heightmap, TerrainBrush* brush, double force, void* data, BrushCallback callback)
{
    IntegerRect brush_rect = _getBrushRect(brush);
    int x, z;
    double dx, dz, distance, influence;

    for (x = brush_rect.xstart; x <= brush_rect.xend; x++)
    {
        dx = (double)x;
        for (z = brush_rect.zstart; z <= brush_rect.zend; z++)
        {
            dz = (double)z;
            distance = sqrt((brush->relative_x - dx) * (brush->relative_x - dx) + (brush->relative_z - dz) * (brush->relative_z - dz));

            if (distance > brush->hard_radius)
            {
                if (distance <= brush->hard_radius + brush->smoothed_size)
                {
                    influence = (1.0 - (distance - brush->hard_radius) / brush->smoothed_size);
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

            double* dpointer = _getDataPointer(&heightmap->brush_data, x, z, heightmap->terrain, 1);
            *dpointer = callback(heightmap, brush, dx, dz, *dpointer, influence, force, data);
        }
    }
}

static double _applyBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(heightmap);
    UNUSED(brush);
    UNUSED(data);
    UNUSED(x);
    UNUSED(z);

    return basevalue + influence * force;
}

void terrainBrushElevation(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushElevation);
}

static double _applyBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(data);

    double ideal, factor;
    ideal = terrainGetInterpolatedHeight(heightmap->terrain, x + brush->total_radius * 0.5, z, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x - brush->total_radius * 0.5, z, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x, z - brush->total_radius * 0.5, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x, z + brush->total_radius * 0.5, 1);
    ideal /= 4.0;
    factor = influence * force;
    if (factor > 1.0)
    {
        factor = 0.0;
    }
    return basevalue + (ideal - basevalue) * factor;
}

void terrainBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushSmooth);
}

static double _applyBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(heightmap);

    return basevalue + noiseGet2DTotal((NoiseGenerator*)data, x / brush->total_radius, z / brush->total_radius) * influence * force * brush->total_radius;
}

void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
    _applyBrush(heightmap, brush, value, generator, _applyBrushAddNoise);
}

static double _applyBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(brush);
    UNUSED(data);

    double ideal = terrainGetInterpolatedHeight(heightmap->terrain, x, z, 0);
    return basevalue + (ideal - basevalue) * influence * force;
}

void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushReset);
}

void terrainEndBrushStroke(TerrainHeightMap* heightmap)
{
    /* TODO Merge data */
}
