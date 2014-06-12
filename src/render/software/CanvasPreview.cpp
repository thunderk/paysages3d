#include "CanvasPreview.h"

#include "Color.h"
#include "CanvasLiveClient.h"
#include "Mutex.h"

#include <cassert>

CanvasPreview::CanvasPreview()
{
    width = 1;
    height = 1;
    pixels = new Color[1];

    dirty_left = 1;
    dirty_right = -1;
    dirty_down = 1;
    dirty_up = -1;

    lock = new Mutex();
}

CanvasPreview::~CanvasPreview()
{
    delete [] pixels;
    delete lock;
}

void CanvasPreview::setSize(int real_width, int real_height, int preview_width, int preview_height)
{
    lock->acquire();

    delete [] pixels;
    pixels = new Color[preview_width * preview_height];

    width = preview_width;
    height = preview_height;

    dirty_left = width;
    dirty_right = -1;
    dirty_down = height;
    dirty_up = -1;

    lock->release();

    reset();
}

void CanvasPreview::reset()
{
    lock->acquire();

    int n = width * height;
    for (int i = 0; i < n; i++)
    {
        pixels[i] = COLOR_BLACK;
    }

    lock->release();
}

void CanvasPreview::initLive(CanvasLiveClient *client)
{
    client->canvasResized(width, height);
    client->canvasCleared(COLOR_BLACK);

    setAllDirty();
}

void CanvasPreview::updateLive(CanvasLiveClient *client)
{
    int x, y;

    lock->acquire();

    for (y = dirty_down; y <= dirty_up; y++)
    {
        for (x = dirty_left; x <= dirty_right; x++)
        {
            client->canvasPainted(x, y, pixels[y * width + x]);
        }
    }

    dirty_left = width;
    dirty_right = -1;
    dirty_down = height;
    dirty_up = -1;

    lock->release();
}

void CanvasPreview::pushPixel(int real_x, int real_y, const Color &old_color, const Color &new_color)
{
    lock->acquire();

    // TODO Assert-check and transform coordinates
    int x = real_x;
    int y = real_y;
    double fact = 1.0;

    Color* pixel = pixels + (real_y * width + real_x);
    pixel->r = pixel->r - old_color.r * fact + new_color.r * fact;
    pixel->g = pixel->g - old_color.g * fact + new_color.g * fact;
    pixel->b = pixel->b - old_color.b * fact + new_color.b * fact;

    // Set pixel dirty
    if (x < dirty_left)
    {
        dirty_left = x;
    }
    if (x > dirty_right)
    {
        dirty_right = x;
    }
    if (y < dirty_down)
    {
        dirty_down = y;
    }
    if (y > dirty_up)
    {
        dirty_up = y;
    }

    lock->release();
}

void CanvasPreview::setAllDirty()
{
    lock->acquire();

    dirty_left = 0;
    dirty_right = width - 1;
    dirty_down = 0;
    dirty_up = height - 1;

    lock->release();
}
