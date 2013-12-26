#include "AtmosphereDefinition.h"

#include "PackStream.h"
#include "RandomGenerator.h"

AtmosphereDefinition::AtmosphereDefinition(BaseDefinition* parent):
    BaseDefinition(parent)
{
}

AtmosphereDefinition::~AtmosphereDefinition()
{
}

void AtmosphereDefinition::save(PackStream* stream) const
{
    stream->write((int*)&model);
    stream->write(&hour);
    stream->write(&minute);
    sun_color.save(stream);
    stream->write(&sun_radius);
    stream->write(&dome_lighting);
    stream->write(&humidity);
    stream->write(&moon_radius);
    stream->write(&moon_theta);
    stream->write(&moon_phi);

    int star_count = stars.size();
    stream->write(&star_count);
    for (const auto &star : stars)
    {
        star.location.save(stream);
        star.col.save(stream);
        stream->write(&star.radius);
    }
}

void AtmosphereDefinition::load(PackStream* stream)
{
    stream->read((int*)&model);
    stream->read(&hour);
    stream->read(&minute);
    sun_color.load(stream);
    stream->read(&sun_radius);
    stream->read(&dome_lighting);
    stream->read(&humidity);
    stream->read(&moon_radius);
    stream->read(&moon_theta);
    stream->read(&moon_phi);

    int star_count;
    stream->read(&star_count);
    for (int i = 0; i < star_count; i++)
    {
        Star star;

        star.location.load(stream);
        star.col.load(stream);
        stream->read(&star.radius);

        stars.push_back(star);
    }

    validate();
}

void AtmosphereDefinition::copy(BaseDefinition* _destination) const
{
    AtmosphereDefinition* destination = (AtmosphereDefinition*)_destination;

    destination->model = model;
    destination->hour = hour;
    destination->minute = minute;
    destination->sun_color = sun_color;
    destination->sun_radius = sun_radius;
    destination->dome_lighting = dome_lighting;
    destination->humidity = humidity;
    destination->moon_radius = moon_radius;
    destination->moon_theta = moon_theta;
    destination->moon_phi = moon_phi;
    destination->stars = stars;

    destination->validate();
}

void AtmosphereDefinition::validate()
{
    if (hour < 0)
    {
        hour = 0;
    }
    if (hour > 23)
    {
        hour = 23;
    }
    if (minute < 0)
    {
        minute = 0;
    }
    if (minute > 59)
    {
        minute = 59;
    }

    _daytime = (double)hour / 24.0 + (double)minute / 1440.0;
}

void AtmosphereDefinition::applyPreset(AtmospherePreset preset)
{
    sun_color.r = 1.0;
    sun_color.g = 0.95;
    sun_color.b = 0.9;
    sun_color.a = 1.0;
    sun_radius = 1.0;
    moon_radius = 1.0;
    moon_theta = 0.3;
    moon_phi = 0.5;
    humidity = 0.1;

    model = ATMOSPHERE_MODEL_BRUNETON;

    switch (preset)
    {
        case ATMOSPHERE_PRESET_CLEAR_DAY:
            hour = 15;
            minute = 0;
            dome_lighting = 0.2;
            break;
        case ATMOSPHERE_PRESET_CLEAR_SUNSET:
            hour = 17;
            minute = 45;
            dome_lighting = 0.3;
            sun_radius = 0.03;
            break;
        case ATMOSPHERE_PRESET_HAZY_MORNING:
            hour = 8;
            minute = 30;
            dome_lighting = 0.25;
            humidity = 0.4;
            break;
        case ATMOSPHERE_PRESET_FOGGY:
            hour = 15;
            minute = 0;
            dome_lighting = 0.1;
            humidity = 0.7;
            break;
        case ATMOSPHERE_PRESET_STORMY:
            hour = 15;
            minute = 0;
            dome_lighting = 0.05;
            humidity = 0.9;
            break;
        default:
            ;
    }

    generateStars(5000);

    validate();
}

void AtmosphereDefinition::generateStars(int count)
{
    stars.clear();

    for (int i = 0; i < count; ++i)
    {
        Star star;

        star.location = Vector3((RandomGenerator::random() - 0.5) * 100000.0, (RandomGenerator::random() - 0.5) * 100000.0, (RandomGenerator::random() - 0.5) * 100000.0);
        if (star.location.getNorm() < 30000.0)
        {
            i--;
            continue;
        }
        double brillance = RandomGenerator::random() * 0.05 + 0.1;
        star.col = Color(brillance + RandomGenerator::random() * 0.03, brillance + RandomGenerator::random() * 0.03, brillance + RandomGenerator::random() * 0.03, 1.0);
        star.radius = 30.0 + RandomGenerator::random() * 20.0;

        stars.push_back(star);
    }
}
