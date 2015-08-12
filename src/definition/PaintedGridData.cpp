#include "PaintedGridData.h"

#include <cstring>
#include "PackStream.h"

PaintedGridData::PaintedGridData()
{
    rows_count = 0;
    rows = (HeightMapRow *)malloc(sizeof(HeightMapRow));
    memsize = 0;
}

PaintedGridData::~PaintedGridData()
{
    clear();
    free(rows);
}

void PaintedGridData::copy(PaintedGridData *destination) const
{
    int i, j, n;
    size_t size;

    destination->clear();

    destination->rows_count = this->rows_count;
    if (destination->rows_count > 0)
    {
        size = sizeof(HeightMapRow) * destination->rows_count;
        destination->rows = (HeightMapRow*)realloc(destination->rows, size);
        destination->memsize += size;
        for (i = 0; i < destination->rows_count; i++)
        {
            destination->rows[i].y = this->rows[i].y;
            destination->rows[i].pixel_groups_count = this->rows[i].pixel_groups_count;
            size = sizeof(HeightMapPixelGroup) * destination->rows[i].pixel_groups_count;
            destination->rows[i].pixel_groups = (HeightMapPixelGroup*)malloc(size);
            destination->memsize += size;
            for (j = 0; j < destination->rows[i].pixel_groups_count; j++)
            {
                destination->rows[i].pixel_groups[j].xstart = this->rows[i].pixel_groups[j].xstart;
                destination->rows[i].pixel_groups[j].xend = this->rows[i].pixel_groups[j].xend;
                n = destination->rows[i].pixel_groups[j].xend - destination->rows[i].pixel_groups[j].xstart + 1;
                size = sizeof(double) * n;
                destination->rows[i].pixel_groups[j].height = (double*)malloc(size);
                destination->memsize += size;
                memcpy(destination->rows[i].pixel_groups[j].height, this->rows[i].pixel_groups[j].height, size);
            }
        }
    }
}

void PaintedGridData::save(PackStream *stream) const
{
    int i, j, k;
    stream->write(&rows_count);
    for (i = 0; i < rows_count; i++)
    {
        stream->write(&rows[i].y);
        stream->write(&rows[i].pixel_groups_count);
        for (j = 0; j < rows[i].pixel_groups_count; j++)
        {
            stream->write(&rows[i].pixel_groups[j].xstart);
            stream->write(&rows[i].pixel_groups[j].xend);
            for (k = 0; k < rows[i].pixel_groups[j].xend - rows[i].pixel_groups[j].xstart; k++)
            {
                stream->write(&rows[i].pixel_groups[j].height[k]);
            }
        }
    }
}

void PaintedGridData::load(PackStream *stream)
{
    int i, j, k, n;
    size_t size;

    clear();

    stream->read(&rows_count);
    if (rows_count > 0)
    {
        size = sizeof(HeightMapRow) * rows_count;
        rows = (HeightMapRow*)realloc(rows, size);
        memsize += size;
        for (i = 0; i < rows_count; i++)
        {
            stream->read(&rows[i].y);
            stream->read(&rows[i].pixel_groups_count);
            size = sizeof(HeightMapPixelGroup) * rows[i].pixel_groups_count;
            rows[i].pixel_groups = (HeightMapPixelGroup*)malloc(size);
            memsize += size;
            for (j = 0; j < rows[i].pixel_groups_count; j++)
            {
                stream->read(&rows[i].pixel_groups[j].xstart);
                stream->read(&rows[i].pixel_groups[j].xend);
                n = rows[i].pixel_groups[j].xend - rows[i].pixel_groups[j].xstart;
                size = sizeof(double) * n;
                rows[i].pixel_groups[j].height = (double*)malloc(size);
                memsize += size;
                for (k = 0; k < n; k++)
                {
                    stream->read(&rows[i].pixel_groups[j].height[k]);
                }
            }
        }
    }
}

void PaintedGridData::clear()
{
    int i, j;
    for (i = 0; i < rows_count; i++)
    {
        for (j = 0; j < rows[i].pixel_groups_count; j++)
        {
            free(rows[i].pixel_groups[j].height);
        }
        free(rows[i].pixel_groups);
    }
    rows_count = 0;
    free(rows);
    rows = (HeightMapRow *)malloc(sizeof(HeightMapRow));
    memsize = 0;
}
