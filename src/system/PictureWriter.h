#pragma once

#include "system_global.h"

#include <string>

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT PictureWriter {
  public:
    PictureWriter();
    virtual ~PictureWriter();

    /**
     * Start saving the picture in a file.
     */
    bool save(const string &filepath, int width, int height);

  protected:
    /**
     * Get the (x, y) pixel, in BGRA format
     */
    virtual unsigned int getPixel(int x, int y) = 0;
};
}
}
