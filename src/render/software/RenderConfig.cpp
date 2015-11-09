#include "RenderConfig.h"

RenderConfig::RenderConfig(int width, int height, int antialias, int quality)
    : width(width), height(height), antialias(antialias), quality(quality) {
    if (this->width <= 0) {
        this->width = 400;
    }
    if (this->height <= 0) {
        this->height = this->width * 4 / 3;
    }
    if (this->antialias < 1) {
        this->antialias = 1;
    }
    if (this->antialias > 4) {
        this->antialias = 4;
    }
    if (this->quality < 1) {
        this->quality = 1;
    }
    if (this->quality > 10) {
        this->quality = 10;
    }
}
