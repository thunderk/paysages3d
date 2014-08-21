#include "CanvasPictureWriter.h"

#include <cassert>

#include "Canvas.h"
#include "CanvasPortion.h"
#include "ColorProfile.h"
#include "PackStream.h"

CanvasPictureWriter::CanvasPictureWriter(const Canvas *canvas):
    canvas(canvas)
{
    profile = new ColorProfile();
    antialias = 1;
    width = canvas->getWidth();
    height = canvas->getHeight();

    last_portion = NULL;
    last_stream = NULL;
    last_y = 0;
}

CanvasPictureWriter::~CanvasPictureWriter()
{
    delete profile;
    if (last_stream)
    {
        delete last_stream;
    }
}

void CanvasPictureWriter::setAntialias(int antialias)
{
    assert(antialias >= 1);
    assert(canvas->getWidth() % antialias == 0);
    assert(canvas->getHeight() % antialias == 0);

    this->antialias = antialias;
    this->width = canvas->getWidth() / antialias;
    this->height = canvas->getHeight() / antialias;
}

void CanvasPictureWriter::setColorProfile(const ColorProfile &profile)
{
    profile.copy(this->profile);
}

bool CanvasPictureWriter::saveCanvas(const std::string &filepath)
{
    return save(filepath, width, height);
}

unsigned int CanvasPictureWriter::getPixel(int x, int y)
{
    Color comp;

    if (antialias > 1)
    {
        int basex = x * antialias;
        int basey = y * antialias;
        double factor = 1.0 / (antialias * antialias);

        comp = COLOR_BLACK;

        for (int iy = 0; iy < antialias; iy++)
        {
            for (int ix = 0; ix < antialias; ix++)
            {
                Color col = getRawPixel(basex + ix, basey + iy);
                comp.r += col.r * factor;
                comp.g += col.g * factor;
                comp.b += col.b * factor;
            }
        }
    }
    else
    {
        comp = getRawPixel(x, y);
    }

    comp = profile->apply(comp);
    comp.normalize();
    return comp.to32BitBGRA();
}

Color CanvasPictureWriter::getRawPixel(int x, int y)
{
    // Get the portion this pixel is in
    CanvasPortion *portion = canvas->atPixel(x, y);

    // While we stay in the same portion line, read is sequential in the stream
    if (portion != last_portion or last_y != y)
    {
        // Get the pack stream positioned at the pixel
        if (last_stream)
        {
            delete last_stream;
        }
        last_stream = new PackStream;
        if (portion->getReadStream(*last_stream, x - portion->getXOffset(), y - portion->getYOffset()))
        {
            last_portion = portion;
            last_y = y;
        }
        else
        {
            // Portion has no stream
            return COLOR_BLACK;
        }
    }

    // Load the pixel
    Color col;
    col.load(last_stream);
    return col;
}
