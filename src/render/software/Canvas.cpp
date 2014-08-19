#include "Canvas.h"

#include <cassert>

#include "CanvasPortion.h"
#include "CanvasPreview.h"

Canvas::Canvas()
{
    horizontal_portion_count = 1;
    vertical_portion_count = 1;
    width = 1;
    height = 1;
    portions.push_back(new CanvasPortion());

    preview = new CanvasPreview;
}

Canvas::~Canvas()
{
    for (auto portion: portions)
    {
        delete portion;
    }
    delete preview;
}

void Canvas::setSize(int width, int height)
{
    horizontal_portion_count = 1 + (width - 1) / 400;
    vertical_portion_count = 1 + (height - 1) / 400;

    int portion_width = width / horizontal_portion_count;
    int portion_height = height / vertical_portion_count;

    for (auto portion: portions)
    {
        delete portion;
    }
    portions.clear();

    int done_width = 0;
    int done_height = 0;

    for (int y = 0; y < vertical_portion_count; y++)
    {
        done_width = 0;
        for (int x = 0; x < horizontal_portion_count; x++)
        {
            CanvasPortion *portion = new CanvasPortion(preview);

            portion->setSize((x == horizontal_portion_count - 1) ? width - done_width : portion_width,
                             (y == vertical_portion_count - 1) ? height - done_height : portion_height,
                             done_width,
                             done_height);

            done_width += portion->getWidth();
            if (x == horizontal_portion_count - 1)
            {
                done_height += portion->getHeight();
            }

            portions.push_back(portion);
        }
        assert(done_width == width);
    }
    assert(done_height == height);

    this->width = width;
    this->height = height;

    // Smaller preview
    while (width > 1000 or height > 700)
    {
        width = width / 2;
        height = height / 2;
    }
    preview->setSize(this->width, this->height, width, height);
}

CanvasPortion *Canvas::at(int x, int y) const
{
    assert(x >= 0 && x < horizontal_portion_count);
    assert(y >= 0 && y < vertical_portion_count);

    return portions[y * horizontal_portion_count + x];
}
