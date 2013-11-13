#include "Color.inline.cpp"

#include <cmath>
#include "PackStream.h"

const Color paysages::basics::COLOR_TRANSPARENT = {0.0, 0.0, 0.0, 0.0};
const Color paysages::basics::COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};
const Color paysages::basics::COLOR_RED = {1.0, 0.0, 0.0, 1.0};
const Color paysages::basics::COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
const Color paysages::basics::COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
const Color paysages::basics::COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
const Color paysages::basics::COLOR_GREY = {0.5, 0.5, 0.5, 1.0};

void Color::save(PackStream* stream) const
{
    stream->write(&r);
    stream->write(&g);
    stream->write(&b);
    stream->write(&a);
}

void Color::load(PackStream* stream)
{
    stream->read(&r);
    stream->read(&g);
    stream->read(&b);
    stream->read(&a);
}
