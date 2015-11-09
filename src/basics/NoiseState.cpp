#include "NoiseState.h"

#include "PackStream.h"
#include "RandomGenerator.h"

NoiseState::NoiseState() {
    for (int i = 0; i < 30; i++) {
        level_offsets.push_back(NoiseOffset());
    }
    randomizeOffsets();
}

void NoiseState::save(PackStream *stream) const {
    int levels = level_offsets.size();
    stream->write(&levels);
    for (const auto &level_offset : level_offsets) {
        stream->write(&level_offset.x);
        stream->write(&level_offset.y);
        stream->write(&level_offset.z);
    }
}

void NoiseState::load(PackStream *stream) {
    int levels;
    stream->read(&levels);
    level_offsets.clear();

    if (levels > 1000) {
        levels = 1000;
    }

    for (int i = 0; i < levels; i++) {
        NoiseOffset level_offset;
        stream->read(&level_offset.x);
        stream->read(&level_offset.y);
        stream->read(&level_offset.z);
        level_offsets.push_back(level_offset);
    }
}

void NoiseState::copy(NoiseState *destination) const {
    destination->level_offsets = level_offsets;
}

void NoiseState::randomizeOffsets() {
    for (auto &level_offset : level_offsets) {
        level_offset.x = RandomGenerator::random();
        level_offset.y = RandomGenerator::random();
        level_offset.z = RandomGenerator::random();
    }
}

void NoiseState::resetOffsets(double x, double y, double z) {
    for (auto &level_offset : level_offsets) {
        level_offset.x = x;
        level_offset.y = y;
        level_offset.z = z;
    }
}

void NoiseState::setLevel(int level, double x, double y, double z) {
    NoiseOffset offset = {x, y, z};
    level_offsets.at(level) = offset;
}

void NoiseState::setLevelCount(int level_count) {
    level_offsets.resize(level_count);
}
