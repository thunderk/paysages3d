#include "TerrainHeightMap.h"

#include <cassert>
#include <cstring>
#include "PackStream.h"
#include "Memory.h"
#include "TerrainDefinition.h"
#include "TerrainHeightMapBrush.h"
#include "Interpolation.h"
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

namespace paysages {
namespace definition {
class TerrainHeightMapData
{
public:
    int memsize;
    int rows_count;
    HeightMapRow* rows;
};
}
}

static void _initData(TerrainHeightMapData* data)
{
    data->rows_count = 0;
    data->rows = new HeightMapRow[1];
    data->memsize = 0;
}

static void _clearData(TerrainHeightMapData* data)
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

static void _deleteData(TerrainHeightMapData* data)
{
    _clearData(data);
    delete[] data->rows;
    delete data;
}

static void _copyData(TerrainHeightMapData* source, TerrainHeightMapData* destination)
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

static void _saveData(PackStream* stream, TerrainHeightMapData* data)
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

static void _loadData(PackStream* stream, TerrainHeightMapData* data)
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
inline double* TerrainHeightMap::getDataPointer(TerrainHeightMapData* data, int x, int z, TerrainHeightMapData* fallback, TerrainDefinition* terrain, int grow)
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
        row = (HeightMapRow*)Memory::naiveArrayInsert((void**)&data->rows, sizeof(HeightMapRow), data->rows_count, i);

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
        pixel_group = (HeightMapPixelGroup*)Memory::naiveArrayInsert((void**)&row->pixel_groups, sizeof(HeightMapPixelGroup), row->pixel_groups_count, i);

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
        pixel = (double*)Memory::naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, 0);
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
        pixel = (double*)Memory::naiveArrayInsert((void**)&pixel_group->height, sizeof(double), pixel_group->xend - pixel_group->xstart, pixel_group->xend - pixel_group->xstart);
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
            double* dpointer = getDataPointer(fallback, x, z, NULL, terrain, 0);
            if (dpointer)
            {
                *pixel = *dpointer;
            }
            else if (terrain)
            {
                *pixel = terrain->getGridHeight(x, z, 0);
            }
        }
        else if (terrain)
        {
            *pixel = terrain->getGridHeight(x, z, 0);
        }
    }
    return pixel;
}

TerrainHeightMap::TerrainHeightMap(TerrainDefinition* terrain):
    BaseDefinition(terrain), terrain(terrain)
{
    merged_data = new TerrainHeightMapData;
    brush_data = new TerrainHeightMapData;
    _initData(merged_data);
    _initData(brush_data);
}

TerrainHeightMap::~TerrainHeightMap()
{
    _deleteData(merged_data);
    _deleteData(brush_data);
}

void TerrainHeightMap::copy(BaseDefinition* _destination) const
{
    TerrainHeightMap* destination = (TerrainHeightMap*)_destination;

    destination->terrain = terrain;

    _copyData(merged_data, destination->merged_data);
    _clearData(destination->brush_data);
}

void TerrainHeightMap::save(PackStream* stream) const
{
    _saveData(stream, merged_data);
}

void TerrainHeightMap::load(PackStream* stream)
{
    _loadData(stream, merged_data);
    _clearData(brush_data);
}

int TerrainHeightMap::getGridHeight(int x, int z, double* result)
{
    double* dpointer;
    dpointer = getDataPointer(brush_data, x, z, NULL, NULL, 0);
    if (dpointer)
    {
        *result = *dpointer;
        return 1;
    }
    else
    {
        dpointer = getDataPointer(merged_data, x, z, NULL, NULL, 0);
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

int TerrainHeightMap::getInterpolatedHeight(double x, double z, double* result)
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
            if (getDataPointer(brush_data, x, z, NULL, NULL, 0) || getDataPointer(merged_data, x, z, NULL, NULL, 0))
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
                if (!getGridHeight(ix, iz, &value))
                {
                    value = terrain->getGridHeight(ix, iz, 0);
                }
                stencil[(iz - (zlow - 1)) * 4 + ix - (xlow - 1)] = value;
            }
        }

        *result = Interpolation::bicubic(stencil, x - (double)xlow, z - (double)zlow);
    }

    return hit;
}

unsigned long TerrainHeightMap::getMemoryStats() const
{
    return merged_data->memsize + brush_data->memsize;
}

bool TerrainHeightMap::isPainted(int x, int z)
{
    return getDataPointer(brush_data, x, z, NULL, NULL, 0) || getDataPointer(merged_data, x, z, NULL, NULL, 0);
}

void TerrainHeightMap::clearPainting()
{
    _clearData(merged_data);
    _clearData(brush_data);
}

void TerrainHeightMap::brushElevation(const TerrainHeightMapBrush &brush, double value)
{
    TerrainHeightMapBrushElevation sbrush(brush);
    sbrush.apply(this, value);
}

void TerrainHeightMap::brushFlatten(const TerrainHeightMapBrush &brush, double height, double force)
{
    TerrainHeightMapBrushFlatten sbrush(brush, height);
    sbrush.apply(this, force);
}

void TerrainHeightMap::brushSmooth(const TerrainHeightMapBrush &brush, double value)
{
    TerrainHeightMapBrushSmooth sbrush(brush);
    sbrush.apply(this, value);
}

void TerrainHeightMap::brushAddNoise(const TerrainHeightMapBrush &brush, NoiseGenerator* generator, double value)
{
    TerrainHeightMapBrushAddNoise sbrush(brush, generator);
    sbrush.apply(this, value);
}

void TerrainHeightMap::brushReset(const TerrainHeightMapBrush &brush, double value)
{
    TerrainHeightMapBrushReset sbrush(brush);
    sbrush.apply(this, value);
}

void TerrainHeightMap::endBrushStroke()
{
    int i, j, k;
    TerrainHeightMapData* data = brush_data;

    for (i = 0; i < data->rows_count; i++)
    {
        for (j = 0; j < data->rows[i].pixel_groups_count; j++)
        {
            for (k = 0; k < data->rows[i].pixel_groups[j].xend - data->rows[i].pixel_groups[j].xstart + 1; k++)
            {
                double* dpointer = getDataPointer(merged_data, data->rows[i].pixel_groups[j].xstart + k, data->rows[i].z, NULL, NULL, 1);
                *dpointer = data->rows[i].pixel_groups[j].height[k];
            }
        }
    }

    _clearData(brush_data);
}
