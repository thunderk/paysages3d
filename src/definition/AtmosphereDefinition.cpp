#include "AtmosphereDefinition.h"

#include <cmath>
#include "PackStream.h"
#include "RandomGenerator.h"
#include "FloatNode.h"
#include "GodRaysDefinition.h"

AtmosphereDefinition::AtmosphereDefinition(DefinitionNode *parent)
    : DefinitionNode(parent, "atmosphere", "atmosphere") {
    model = ATMOSPHERE_MODEL_DISABLED;
    godrays = new GodRaysDefinition(this);
    daytime = new FloatNode(this, "daytime");
    humidity = new FloatNode(this, "humidity");
    sun_radius = new FloatNode(this, "sun_radius");
    sun_color = COLOR_RED;
}

AtmosphereDefinition::~AtmosphereDefinition() {
}

void AtmosphereDefinition::save(PackStream *stream) const {
    DefinitionNode::save(stream);

    stream->write((int *)&model);
    sun_color.save(stream);
    stream->write(&dome_lighting);
    stream->write(&moon_radius);
    stream->write(&moon_theta);
    stream->write(&moon_phi);

    int star_count = stars.size();
    stream->write(&star_count);
    for (const auto &star : stars) {
        star.location.save(stream);
        star.col.save(stream);
        stream->write(&star.radius);
    }
}

void AtmosphereDefinition::load(PackStream *stream) {
    DefinitionNode::load(stream);

    stream->read((int *)&model);
    sun_color.load(stream);
    stream->read(&dome_lighting);
    stream->read(&moon_radius);
    stream->read(&moon_theta);
    stream->read(&moon_phi);

    int star_count;
    stream->read(&star_count);
    for (int i = 0; i < star_count; i++) {
        Star star;

        star.location.load(stream);
        star.col.load(stream);
        stream->read(&star.radius);

        stars.push_back(star);
    }

    validate();
}

void AtmosphereDefinition::copy(DefinitionNode *_destination) const {
    DefinitionNode::copy(_destination);

    AtmosphereDefinition *destination = (AtmosphereDefinition *)_destination;

    destination->model = model;
    destination->sun_color = sun_color;
    destination->dome_lighting = dome_lighting;
    destination->moon_radius = moon_radius;
    destination->moon_theta = moon_theta;
    destination->moon_phi = moon_phi;
    destination->stars = stars;

    destination->validate();
}

void AtmosphereDefinition::setDayTime(double value) {
    daytime->setValue(value);
}

void AtmosphereDefinition::setDayTime(int hour, int minute, int second) {
    setDayTime(to_double(hour) / 24.0 + to_double(minute) / 1440.0 + to_double(second) / 86400.0);
}

void AtmosphereDefinition::getHMS(int *hour, int *minute, int *second) const {
    double value = daytime->getValue();
    if (value >= 0.0) {
        value = fmod(value, 1.0);
    } else {
        value = 1.0 - fmod(-value, 1.0);
    }
    value *= 86400.0;
    *hour = value / 3600.0;
    value -= 3600.0 * *hour;
    *minute = value / 60.0;
    *second = value - *minute * 60.0;
}

void AtmosphereDefinition::applyPreset(AtmospherePreset preset, RandomGenerator &random) {
    sun_color.r = 1.0;
    sun_color.g = 0.95;
    sun_color.b = 0.9;
    sun_color.a = 1.0;
    sun_radius->setValue(0.7);
    moon_radius = 1.0;
    moon_theta = 0.3;
    moon_phi = 0.5;

    model = ATMOSPHERE_MODEL_BRUNETON;

    switch (preset) {
    case ATMOSPHERE_PRESET_CLEAR_DAY:
        setDayTime(15);
        humidity->setValue(0.1);
        dome_lighting = 0.2;
        break;
    case ATMOSPHERE_PRESET_CLEAR_SUNSET:
        setDayTime(17, 45);
        humidity->setValue(0.1);
        dome_lighting = 0.3;
        break;
    case ATMOSPHERE_PRESET_HAZY_MORNING:
        setDayTime(8, 30);
        humidity->setValue(0.4);
        dome_lighting = 0.25;
        break;
    case ATMOSPHERE_PRESET_FOGGY:
        setDayTime(15);
        humidity->setValue(0.7);
        dome_lighting = 0.1;
        break;
    case ATMOSPHERE_PRESET_STORMY:
        setDayTime(15);
        humidity->setValue(0.9);
        dome_lighting = 0.05;
        break;
    default:
        ;
    }

    generateStars(2000, random);

    validate();
}

void AtmosphereDefinition::generateStars(int count, RandomGenerator &random) {
    stars.clear();

    for (int i = 0; i < count; ++i) {
        Star star;

        star.location = Vector3((random.genDouble() - 0.5) * 100000.0, (random.genDouble() * 0.5) * 100000.0,
                                (random.genDouble() - 0.5) * 100000.0);
        if (star.location.getNorm() < 30000.0) {
            i--;
            continue;
        }
        double brillance = random.genDouble() * 0.05 + 0.1;
        star.col = Color(brillance + random.genDouble() * 0.03, brillance + random.genDouble() * 0.03,
                         brillance + random.genDouble() * 0.03, 1.0);
        star.radius = 30.0 + random.genDouble() * 20.0;

        stars.push_back(star);
    }
}
