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
#include "PackStream.h"
#include "NoiseGenerator.h"

typedef struct
{
    int xstart;
    int xend;
    double* height;
} HeightMapPixelGroup;

typedef struct
{
    int z;
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
    data->rows = new HeightMapRow[1];
    data->memsize = 0;
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
    delete[] data->rows;
    data->rows = new HeightMapRow[1];
    data->memsize = 0;
}

static void _deleteData(HeightMapData* data)
{
    _clearData(data);
    delete[] data->rows;
}

static void _copyData(HeightMapData* source, HeightMapData* destination)
{
    int i, j, n;
    size_t size;

    _clearData(destination);

    destination->rows_count = source->rows_count;
    if (destination->rows_count > 0)
    {
        size = sizeof(HeightMapRow) * destination->rows_count;
        destination->rows = (HeightMapRow*)realloc(destination->rows, size);
        destination->memsize += size;
        for (i = 0; i < destination->rows_count; i++)
        {
            destination->rows[i].z = source->rows[i].z;
            destination->rows[i].pixel_groups_count = source->rows[i].pixel_groups_count;
            size = sizeof(HeightMapPixelGroup) * destination->rows[i].pixel_groups_count;
            destination->rows[i].pixel_groups = (HeightMapPixelGroup*)malloc(size);
            destination->memsize += size;
            for (j = 0; j < destination->rows[i].pixel_groups_count; j++)
            {
                destination->rows[i].pixel_groups[j].xstart = source->rows[i].pixel_groups[j].xstart;
                destination->rows[i].pixel_groups[j].xend = source->rows[i].pixel_groups[j].xend;
                n = destination->rows[i].pixel_groups[j].xend - destination->rows[i].pixel_groups[j].xstart + 1;
                size = sizeof(double) * n;
                destination->rows[i].pixel_groups[j].height = (double*)malloc(size);
                destination->memsize += size;
                memcpy(destination->rows[i].pixel_groups[j].height, source->rows[i].pixel_groups[j].height, size);
            }
        }
    }
}

static void _saveData(PackStream* stream, HeightMapData* data)
{
    int i, j, k;
    stream->write(&data->rows_count);
    for (i = 0; i < data->rows_count; i++)
    {
        stream->write(&data->rows[i].z);
        stream->write(&data->rows[i].pixel_groups_count);
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            stream->write(&data->rows[i].pixel_groups[j].xstart);
            stream->write(&data->rows[i].pixel_groups[j].xend);
            for (k = 0; k < data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart; k++)
            {
                stream->write(&data->rows[i].pixel_groups[j].height[k]);
            }
        }
    }
}

static void _loadData(PackStream* stream, HeightMapData* data)
{
    int i, j, k, n;
    size_t size;

    _clearData(data);

    stream->read(&data->rows_count);
    if (data->rows_count > 0)
    {
        size = sizeof(HeightMapRow) * data->rows_count;
        data->rows = (HeightMapRow*)realloc(data->rows, size);
        data->memsize += size;
        for (i = 0; i < data->rows_count; i++)
        {
            stream->read(&data->rows[i].z);
            stream->read(&data->rows[i].pixel_groups_count);
            size = sizeof(HeightMapPixelGroup) * data->rows[i].pixel_groups_count;
            data->rows[i].pixel_groups = (HeightMapPixelGroup*)malloc(size);
            data->memsize += size;
            for (j = 0; j < data->rows[i].pixel_groups_count; j++)
            {
                stream->read(&data->rows[i].pixel_groups[j].xstart);
                stream->read(&data->rows[i].pixel_groups[j].xend);
                n = data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart;
                size = sizeof(double) * n;
                data->rows[i].pixel_groups[j].height = (double*)malloc(size);
                data->memsize += size;
                for (k = 0; k < n; k++)
                {
                    stream->read(&data->rows[i].pixel_groups[j].height[k]);
                }
            }
        }
    }
}

/*
 * Get a pointer to the data in a heightmap, to a certain location.
 * If the location is not already in the heightmap, it is initialized with the terrain height.
 * This method will grow the heightmap as necessary (if 'grow' is set to false, NULL will be returned on missing pixels).
 */
static double* _getDataPointer(HeightMapData* data, int x, int z, HeightMapData* fallback, TerrainDefinition* terrain, int grow)
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
        row = (HeightMapRow*)naiveArrayInsert((void**)&data->rows, sizeof(HeightMapRow), data->rows_count, i);

        row->z = z;
        row->pixel_groups_count = 0;
        row->pixel_groups = (HeightMapPixelGroup*)malloc(1);

        data->rows_count++;
        data->memsize += sizeof(HeightMapRow);
    }
    else
    {
        return NULL;
    }

#ifndef NDEBUG
    /* Check rows */
    for (i = 1; i < data->rows_count; i++)
    {
        assert(data->rows[i].z > data->rows[i - 1].z);
    }
#endif

    /* Find pixel group */
    HeightMapPixelGroup* pixel_group = NULL;
    for (i = 0; i < row->pixel_groups_count; i++)
    {
        if (x < row->pixel_groups[i].xstart - 1)
        {
            break;
        }
        else if (x <= row->pixel_groups[i].xend + 1)
        {
            if (x == row->pixel_groups[i].xend + 1 && i < row->pixel_groups_count - 1 && x == row->pixel_groups[i + 1].xstart)
            {
                /* Choose next group if it already includes the pixel */
                i++;
            }
            pixel_group = row->pixel_groups + i;
            break;
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
        pixel_group = (HeightMapPixelGroup*)naiveArrayInsert((void**)&row->pixel_groups, sizeof(HeightMapPixelGroup), row->pixel_groups_count, i);

        pixel_group->xstart = x;
        pixel_group->xend = x;
        pixel_group->height = (double*)malloc(sizeof(double));

        pixel = pixel_group->height;

        row->pixel_groups_count++;
        data->memsize += sizeof(HeightMapPixelGroup) + sizeof(double);
    }
    else if (x == pixel_group->xstart - 1)
    {
        if (!grow)
        {
            return NULL;
        }

        /* Extend the rowgroup at start */
        pixel_group->xstart--;
        pixel = (double*)naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, 0);
        data->memsize += sizeof(double);
    }
    else if (x == pixel_group->xend + 1)
    {
        if (!grow)
        {
            return NULL;
        }

        /* Extend the rowgroup at end */
        pixel_group->xend++;
        pixel = (double*)naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, pixel_group->xend - pixel_group->xstart);
        data->memsize += sizeof(double);
    }
    else
    {
        assert(x >= pixel_group->xstart);
        assert(x <= pixel_group->xend);
        pixel = pixel_group->height + x - pixel_group->xstart;
        added = 0;
    }

#ifndef NDEBUG
    /* Check pixel groups */
    for (i = 0; i < row->pixel_groups_count; i++)
    {
        if (i > 0)
        {
            assert(row->pixel_groups[i].xstart > row->pixel_groups[i - 1].xend);
        }
        if (i < row->pixel_groups_count - 1)
        {
            assert(row->pixel_groups[i].xend < row->pixel_groups[i + 1].xstart);
        }
        assert(row->pixel_groups[i].xend >= row->pixel_groups[i].xstart);
    }
#endif

    /* Reset pixel if it had been added */
    if (added && (terrain || fallback))
    {
        if (fallback)
        {
            double* dpointer = _getDataPointer(fallback, x, z, NULL, terrain, 0);
            if (dpointer)
            {
                *pixel = *dpointer;
            }
            else if (terrain)
            {
                *pixel = terrainGetGridHeight(terrain, x, z, 0);
            }
        }
        else if (terrain)
        {
            *pixel = terrainGetGridHeight(terrain, x, z, 0);
        }
    }
    return pixel;
}

TerrainHeightMap* terrainHeightMapCreate(TerrainDefinition* terrain)
{
    TerrainHeightMap* result;

    result = new TerrainHeightMap;
    result->terrain = terrain;
    _initData(&result->merged_data);
    _initData(&result->brush_data);

    return result;
}

void terrainHeightmapDelete(TerrainHeightMap* heightmap)
{
    _deleteData(&heightmap->merged_data);
    _deleteData(&heightmap->brush_data);
    delete heightmap;
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

int terrainHeightmapGetGridHeight(TerrainHeightMap* heightmap, int x, int z, double* result)
{
    double* dpointer;
    dpointer = _getDataPointer(&heightmap->brush_data, x, z, NULL, NULL, 0);
    if (dpointer)
    {
        *result = *dpointer;
        return 1;
    }
    else
    {
        dpointer = _getDataPointer(&heightmap->merged_data, x, z, NULL, NULL, 0);
        if (dpointer)
        {
            *result = *dpointer;
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

int terrainHeightmapGetInterpolatedHeight(TerrainHeightMap* heightmap, double x, double z, double* result)
{
    int ix, iz;
    int xlow;
    int zlow;

    xlow = floor(x);
    zlow = floor(z);

    int hit = 0;
    for (ix = xlow - 1; ix <= xlow + 2 && !hit; ix++)
    {
        for (iz = zlow - 1; iz <= zlow + 2 && !hit; iz++)
        {
            if (_getDataPointer(&heightmap->brush_data, x, z, NULL, NULL, 0) || _getDataPointer(&heightmap->merged_data, x, z, NULL, NULL, 0))
            {
                hit = 1;
            }
        }
    }

    if (hit && result)
    {
        double stencil[16];
        double value;
        for (ix = xlow - 1; ix <= xlow + 2; ix++)
        {
            for (iz = zlow - 1; iz <= zlow + 2; iz++)
            {
                if (!terrainHeightmapGetGridHeight(heightmap, ix, iz, &value))
                {
                    value = terrainGetGridHeight(heightmap->terrain, ix, iz, 0);
                }
                stencil[(iz - (zlow - 1)) * 4 + ix - (xlow - 1)] = value;
            }
        }

        *result = toolsBicubicInterpolate(stencil, x - (double)xlow, z - (double)zlow);
    }

    return hit;
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
    return definition->height_map->merged_data.memsize + definition->height_map->brush_data.memsize;
}

int terrainIsPainted(TerrainHeightMap* heightmap, int x, int z)
{
    return _getDataPointer(&heightmap->brush_data, x, z, NULL, NULL, 0) || _getDataPointer(&heightmap->merged_data, x, z, NULL, NULL, 0);
}

void terrainClearPainting(TerrainHeightMap* heightmap)
{
    _clearData(&heightmap->merged_data);
    _clearData(&heightmap->brush_data);
}

typedef double (*BrushCallback)(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data);

static inline void _applyBrush(TerrainHeightMap* heightmap, TerrainBrush* brush, double force, void* data, BrushCallback callback)
{
    IntegerRect brush_rect = _getBrushRect(brush);
    int x, z;
    double dx, dz, distance, influence;

    force /= heightmap->terrain->height;

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

            double* dpointer = _getDataPointer(&heightmap->brush_data, x, z, &heightmap->merged_data, heightmap->terrain, 1);
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

static double _applyBrushFlatten(TerrainHeightMap* heightmap, TerrainBrush* brush, double, double, double basevalue, double influence, double force, void* data)
{
    UNUSED(heightmap);
    UNUSED(brush);
    UNUSED(data);

    double ideal = *((double*)data);
    return basevalue + (ideal - basevalue) * influence * force;
}

void terrainBrushFlatten(TerrainHeightMap* heightmap, TerrainBrush* brush, double height, double force)
{
    _applyBrush(heightmap, brush, force, &height, _applyBrushFlatten);
}

static double _applyBrushSmooth(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(data);

    double ideal, factor;
    ideal = terrainGetInterpolatedHeight(heightmap->terrain, (x + brush->total_radius * 0.5) * heightmap->terrain->scaling, z * heightmap->terrain->scaling, 0, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, (x - brush->total_radius * 0.5) * heightmap->terrain->scaling, z * heightmap->terrain->scaling, 0, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x * heightmap->terrain->scaling, (z - brush->total_radius * 0.5) * heightmap->terrain->scaling, 0, 1);
    ideal += terrainGetInterpolatedHeight(heightmap->terrain, x * heightmap->terrain->scaling, (z + brush->total_radius * 0.5) * heightmap->terrain->scaling, 0, 1);
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

static double _applyBrushAddNoise(TerrainHeightMap*, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    NoiseGenerator* noise = (NoiseGenerator*)data;

    return basevalue + noise->get2DTotal(x / brush->total_radius, z / brush->total_radius) * influence * force * brush->total_radius;
}

void terrainBrushAddNoise(TerrainHeightMap* heightmap, TerrainBrush* brush, NoiseGenerator* generator, double value)
{
    _applyBrush(heightmap, brush, value, generator, _applyBrushAddNoise);
}

static double _applyBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double x, double z, double basevalue, double influence, double force, void* data)
{
    UNUSED(brush);
    UNUSED(data);

    double ideal = terrainGetInterpolatedHeight(heightmap->terrain, x * heightmap->terrain->scaling, z * heightmap->terrain->scaling, 0, 0);
    return basevalue + (ideal - basevalue) * influence * force;
}

void terrainBrushReset(TerrainHeightMap* heightmap, TerrainBrush* brush, double value)
{
    _applyBrush(heightmap, brush, value, NULL, _applyBrushReset);
}

void terrainEndBrushStroke(TerrainHeightMap* heightmap)
{
    int i, j, k;
    HeightMapData* data = &heightmap->brush_data;

    for (i = 0; i < data->rows_count; i++)
    {
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            for (k = 0; k < data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart + 1; k++)
            {
                double* dpointer = _getDataPointer(&heightmap->merged_data, data->rows[i].pixel_groups[j].xstart + k, data->rows[i].z, NULL, NULL, 1);
                *dpointer = data->rows[i].pixel_groups[j].height[k];
            }
        }
    }

    _clearData(&heightmap->brush_data);
}
