#ifndef PICTUREWRITER_H
#define PICTUREWRITER_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT PictureWriter {
  public:
    PictureWriter();
    virtual ~PictureWriter();

    /**
     * @brief Start saving the picture in a file.
     */
    bool save(const string &filepath, int width, int height);

  protected:
    /**
     * @brief Get the (x, y) pixel, in BGRA format
     */
    virtual unsigned int getPixel(int x, int y) = 0;
};
}
}

#endif // PICTUREWRITER_H
