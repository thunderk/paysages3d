#include "CanvasPreview.h"

#include "CanvasLiveClient.h"
#include "Color.h"
#include "ColorProfile.h"
#include "Mutex.h"

#include <cassert>

#define CHECK_COORDINATES(_x_, _y_)                                                                                    \
    assert(_x_ >= 0);                                                                                                  \
    assert(_y_ >= 0);                                                                                                  \
    assert(_x_ < this->width);                                                                                         \
    assert(_y_ < this->height)

CanvasPreview::CanvasPreview() {
    width = 1;
    height = 1;
    pixels = new Color[1];

    dirty_left = 1;
    dirty_right = -1;
    dirty_down = 1;
    dirty_up = -1;

    scaled = false;
    factor = 1.0;
    factor_x = 1.0;
    factor_y = 1.0;

    lock = new Mutex();
    profile = new ColorProfile();
}

CanvasPreview::~CanvasPreview() {
    delete[] pixels;
    delete lock;
    delete profile;
}

const Color &CanvasPreview::getFinalPixel(int x, int y) const {
    CHECK_COORDINATES(x, y);

    return pixels[y * width + x];
}

void CanvasPreview::setSize(int real_width, int real_height, int preview_width, int preview_height) {
    lock->acquire();

    delete[] pixels;
    pixels = new Color[preview_width * preview_height];

    width = preview_width;
    height = preview_height;

    dirty_left = width;
    dirty_right = -1;
    dirty_down = height;
    dirty_up = -1;

    scaled = (real_width != preview_height or real_height != preview_height);
    factor_x = to_double(preview_width) / to_double(real_width);
    factor_y = to_double(preview_height) / to_double(real_height);
    factor = factor_x * factor_y;

    lock->release();

    reset();
}

void CanvasPreview::setToneMapping(const ColorProfile &profile) {
    profile.copy(this->profile);
    setAllDirty();
}

void CanvasPreview::reset() {
    lock->acquire();

    int n = width * height;
    for (int i = 0; i < n; i++) {
        pixels[i] = COLOR_BLACK;
    }

    lock->release();
}

void CanvasPreview::initLive(CanvasLiveClient *client) {
    client->canvasResized(width, height);
    client->canvasCleared(COLOR_BLACK);

    setAllDirty();
}

void CanvasPreview::updateLive(CanvasLiveClient *client) {
    int x, y;

    lock->acquire();

    for (y = dirty_down; y <= dirty_up; y++) {
        for (x = dirty_left; x <= dirty_right; x++) {
            client->canvasPainted(x, y, profile->apply(pixels[y * width + x]));
        }
    }

    dirty_left = width;
    dirty_right = -1;
    dirty_down = height;
    dirty_up = -1;

    lock->release();
}

void CanvasPreview::pushPixel(int real_x, int real_y, const Color &old_color, const Color &new_color) {
    int x, y;

    if (scaled) {
        x = int(real_x * factor_x);
        y = int(real_y * factor_y);

        x = (x >= width) ? width - 1 : x;
        y = (y >= height) ? height - 1 : y;
    } else {
        x = real_x;
        y = real_y;
    }

    CHECK_COORDINATES(x, y);

    lock->acquire();

    Color *pixel = pixels + (y * width + x);
    pixel->r = pixel->r - old_color.r * factor + new_color.r * factor;
    pixel->g = pixel->g - old_color.g * factor + new_color.g * factor;
    pixel->b = pixel->b - old_color.b * factor + new_color.b * factor;

    // Set pixel dirty
    if (x < dirty_left) {
        dirty_left = x;
    }
    if (x > dirty_right) {
        dirty_right = x;
    }
    if (y < dirty_down) {
        dirty_down = y;
    }
    if (y > dirty_up) {
        dirty_up = y;
    }

    lock->release();
}

void CanvasPreview::setAllDirty() {
    lock->acquire();

    dirty_left = 0;
    dirty_right = width - 1;
    dirty_down = 0;
    dirty_up = height - 1;

    lock->release();
}
