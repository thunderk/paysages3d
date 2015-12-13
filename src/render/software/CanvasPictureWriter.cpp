#include "CanvasPictureWriter.h"

#include <cassert>

#include "Canvas.h"
#include "CanvasPortion.h"
#include "ColorProfile.h"
#include "PackStream.h"

CanvasPictureWriter::CanvasPictureWriter(const Canvas *canvas) : canvas(canvas) {
    profile = new ColorProfile();
    antialias = 1;
    width = canvas->getWidth();
    height = canvas->getHeight();

    cache = new Color[1];
    cache_y = -antialias;
    cache_width = 0;
}

CanvasPictureWriter::~CanvasPictureWriter() {
    delete profile;
    delete[] cache;
}

void CanvasPictureWriter::setAntialias(int antialias) {
    assert(antialias >= 1);
    assert(canvas->getWidth() % antialias == 0);
    assert(canvas->getHeight() % antialias == 0);

    this->antialias = antialias;
    this->width = canvas->getWidth() / antialias;
    this->height = canvas->getHeight() / antialias;
}

void CanvasPictureWriter::setColorProfile(const ColorProfile &profile) {
    profile.copy(this->profile);
}

bool CanvasPictureWriter::saveCanvas(const string &filepath) {
    return save(filepath, width, height);
}

unsigned int CanvasPictureWriter::getPixel(int x, int y) {
    Color comp;

    if (antialias > 1) {
        int basex = x * antialias;
        int basey = y * antialias;
        double factor = 1.0 / (antialias * antialias);

        comp = COLOR_BLACK;

        for (int iy = 0; iy < antialias; iy++) {
            for (int ix = 0; ix < antialias; ix++) {
                Color col = getRawPixel(basex + ix, basey + iy);
                comp.r += col.r * factor;
                comp.g += col.g * factor;
                comp.b += col.b * factor;
            }
        }
    } else {
        comp = getRawPixel(x, y);
    }

    comp = profile->apply(comp);
    comp.normalize();
    return comp.to32BitBGRA();
}

Color CanvasPictureWriter::getRawPixel(int x, int y) {
    if (not(y >= cache_y && y < cache_y + antialias)) {
        // Load rows into cache
        delete[] cache;
        cache_y = y;
        cache_width = canvas->getWidth();
        cache = new Color[cache_width * antialias];

        CanvasPortion *portion = NULL;
        PackStream *stream = new PackStream;

        Color *itcolor = cache;
        bool has_pixels = false;
        for (int cy = cache_y; cy < cache_y + antialias; cy++) {
            for (int cx = 0; cx < cache_width; cx++) {
                CanvasPortion *nportion = canvas->atPixel(cx, cy);
                if (nportion != portion) {
                    portion = nportion;
                    delete stream;
                    stream = new PackStream;
                    has_pixels =
                        portion->getReadStream(*stream, cx - portion->getXOffset(), cy - portion->getYOffset());
                }
                if (has_pixels) {
                    itcolor->load(stream);
                } else {
                    *itcolor = COLOR_BLACK;
                }
                itcolor++;
            }
        }

        delete stream;
    }

    // Hit the cache
    return cache[(y - cache_y) * cache_width + x];
}
