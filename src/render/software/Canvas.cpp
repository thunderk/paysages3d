#include "Canvas.h"

#include "CanvasPortion.h"
#include <cassert>

Canvas::Canvas()
{
    horizontal_portion_count = 1;
    vertical_portion_count = 1;
    width = 1;
    height = 1;
    preview_width = 1;
    preview_height = 1;
    portions.push_back(new CanvasPortion());
}

Canvas::~Canvas()
{
    for (auto portion: portions)
    {
        delete portion;
    }
}

void Canvas::setSize(int width, int height)
{
    horizontal_portion_count = 1 + width / 400;
    vertical_portion_count = 1 + height / 400;

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
            CanvasPortion *portion = new CanvasPortion();

            portion->setSize((x == horizontal_portion_count - 1) ? width - done_width : portion_width,
                             (y == vertical_portion_count - 1) ? height - done_height : portion_height);

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
    this->preview_width = width;
    this->preview_height = height;
}

CanvasPortion *Canvas::at(int x, int y) const
{
    assert(x >= 0 && x < horizontal_portion_count);
    assert(y >= 0 && y < vertical_portion_count);

    return portions[y * horizontal_portion_count + x];
}
