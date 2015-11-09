#include "CanvasFragment.h"

CanvasFragment::CanvasFragment(bool front_facing, const Vector3 &pixel, const Vector3 &location, int client,
                               bool opaque)
    : opaque(opaque), front_facing(front_facing), pixel(pixel), location(location), client(client) {
    color = COLOR_WHITE;
}

void CanvasFragment::setColor(const Color &col) {
    color = col;
}
