#include "CanvasPortion.h"

#include <cassert>
#include <stdlib.h>
#include <vector>

#include "CanvasPixel.h"
#include "CanvasPreview.h"
#include "PackStream.h"
#include "FileSystem.h"
#include "System.h"

#define CHECK_COORDINATES()                                                                                            \
    assert(x >= 0);                                                                                                    \
    assert(x < width);                                                                                                 \
    assert(y >= 0);                                                                                                    \
    assert(y < height);                                                                                                \
    assert(pixels != NULL)

// Keep track of created files to erase them at program exit
static std::vector<std::string> _files;
static void clean_all_files() {
    for (auto &filepath : _files) {
        FileSystem::removeFile(filepath);
    }
}
static int _atexit = std::atexit(clean_all_files);

CanvasPortion::CanvasPortion(int index, CanvasPreview *preview) : index(index), preview(preview) {
    width = 1;
    height = 1;
    xoffset = 0;
    yoffset = 0;
    pixels = NULL;
}

CanvasPortion::~CanvasPortion() {
    if (pixels) {
        delete[] pixels;
    }
}

int CanvasPortion::getFragmentCount(int x, int y) const {
    CHECK_COORDINATES();

    return pixels[y * width + x].getFragmentCount();
}

const CanvasFragment *CanvasPortion::getFrontFragment(int x, int y) const {
    CHECK_COORDINATES();

    return pixels[y * width + x].getFrontFragment();
}

void CanvasPortion::clear() {
    int n = width * height;
    for (int i = 0; i < n; i++) {
        pixels[i].reset();
    }
}

void CanvasPortion::setSize(int width, int height, int xoffset, int yoffset) {
    this->width = width;
    this->height = height;
    this->xoffset = xoffset;
    this->yoffset = yoffset;
}

void CanvasPortion::preparePixels() {
    if (pixels) {
        delete[] pixels;
    }
    pixels = new CanvasPixel[width * height];
}

void CanvasPortion::discardPixels(bool save) {
    if (pixels) {
        if (save) {
            saveToDisk();
        }
        delete[] pixels;
        pixels = NULL;
    }
}

void CanvasPortion::saveToDisk() {
    if (pixels) {
        auto pid = System::getProcessId();
        filepath =
            FileSystem::getTempFile("paysages_portion_" + std::to_string(index) + "_" + std::to_string(pid) + ".dat");
        PackStream stream;
        stream.bindToFile(filepath, true);
        stream.write(&width);
        stream.write(&height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                pixels[y * width + x].getComposite().save(&stream);
            }
        }
        _files.push_back(filepath);
    }
}

bool CanvasPortion::getReadStream(PackStream &stream, int x, int y) {
    if (FileSystem::isFile(filepath)) {
        if (not stream.bindToFile(filepath)) {
            return false;
        }

        int unused_i;
        stream.skip(unused_i, 2);

        if (x > 0 or y > 0) {
            double unused_d;
            stream.skip(unused_d, (y * width + x) * 4);
        }

        return true;
    } else {
        return false;
    }
}

void CanvasPortion::pushFragment(int x, int y, const CanvasFragment &fragment) {
    CHECK_COORDINATES();

    CanvasPixel &pixel = pixels[y * width + x];
    Color old_color = pixel.getComposite();

    pixel.pushFragment(fragment);

    if (preview) {
        preview->pushPixel(xoffset + x, yoffset + y, old_color, pixel.getComposite());
    }
}

const CanvasPixel &CanvasPortion::at(int x, int y) {
    CHECK_COORDINATES();

    return pixels[y * width + x];
}

void CanvasPortion::setColor(int x, int y, const Color &color) {
    CHECK_COORDINATES();

    CanvasPixel &pixel = pixels[y * width + x];
    Color old_color = pixel.getComposite();

    pixel.setComposite(color);

    if (preview) {
        preview->pushPixel(xoffset + x, yoffset + y, old_color, pixel.getComposite());
    }
}
