#include "PictureWriter.h"

#include <QImage>

PictureWriter::PictureWriter() {
}

PictureWriter::~PictureWriter() {
}

bool PictureWriter::save(const string &filepath, int width, int height) {
    QImage result(width, height, QImage::Format_ARGB32);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            result.setPixel(x, height - 1 - y, getPixel(x, y));
        }
    }

    return result.save(QString::fromStdString(filepath));
}
