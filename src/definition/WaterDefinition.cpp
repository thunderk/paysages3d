#include "WaterDefinition.h"

#include "PackStream.h"
#include "NoiseState.h"
#include "Color.h"
#include "SurfaceMaterial.h"

WaterDefinition::WaterDefinition(BaseDefinition* parent):
    BaseDefinition(parent, "water")
{
    material = new SurfaceMaterial;
    depth_color = new Color;
    foam_material = new SurfaceMaterial;
    noise_state = new NoiseState();

    transparency_depth = 0.0;
    transparency = 0.0;
    reflection = 0.0;
    lighting_depth = 0.0;
    scaling = 1.0;
    waves_height = 0.0;
    detail_height = 0.0;
    turbulence = 0.0;
    foam_coverage = 0.0;
}

WaterDefinition::~WaterDefinition()
{
    delete material;
    delete depth_color;
    delete foam_material;
    delete noise_state;
}

void WaterDefinition::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    material->save(stream);
    depth_color->save(stream);
    stream->write(&transparency_depth);
    stream->write(&transparency);
    stream->write(&reflection);
    stream->write(&lighting_depth);

    stream->write(&scaling);
    stream->write(&waves_height);
    stream->write(&detail_height);
    stream->write(&turbulence);

    stream->write(&foam_coverage);
    foam_material->save(stream);

    noise_state->save(stream);
}

void WaterDefinition::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    material->load(stream);
    depth_color->load(stream);
    stream->read(&transparency_depth);
    stream->read(&transparency);
    stream->read(&reflection);
    stream->read(&lighting_depth);

    stream->read(&scaling);
    stream->read(&waves_height);
    stream->read(&detail_height);
    stream->read(&turbulence);

    stream->read(&foam_coverage);
    foam_material->load(stream);

    noise_state->load(stream);

    validate();
}

void WaterDefinition::copy(BaseDefinition* _destination) const
{
    BaseDefinition::copy(_destination);

    WaterDefinition* destination = (WaterDefinition*)_destination;
    *destination->material = *material;
    *destination->depth_color = *depth_color;
    destination->transparency_depth = transparency_depth;
    destination->transparency = transparency;
    destination->reflection = reflection;
    destination->lighting_depth = lighting_depth;
    destination->scaling = scaling;
    destination->waves_height = waves_height;
    destination->detail_height = detail_height;
    destination->turbulence = turbulence;
    destination->foam_coverage = foam_coverage;
    *destination->foam_material = *foam_material;
    noise_state->copy(destination->noise_state);
}

void WaterDefinition::validate()
{
    BaseDefinition::validate();

    material->validate();
    foam_material->validate();
}

void WaterDefinition::applyPreset(WaterPreset preset)
{
    noise_state->randomizeOffsets();

    if (preset == WATER_PRESET_LAKE)
    {
        transparency = 0.5;
        reflection = 0.2;
        transparency_depth = 4.0;
        material->base = colorToHSL(Color(0.08, 0.15, 0.2, 1.0));
        depth_color->r = 0.0;
        depth_color->g = 0.1;
        depth_color->b = 0.1;
        lighting_depth = 6.0;
        scaling = 1.0;
        waves_height = 0.8;
        detail_height = 0.05;
        turbulence = 0.1;
        foam_coverage = 0.15;
    }
    else if (preset == WATER_PRESET_SEA)
    {
        transparency = 0.3;
        reflection = 0.07;
        transparency_depth = 3.0;
        material->base = colorToHSL(Color(0.05, 0.18, 0.2, 1.0));
        depth_color->r = 0.0;
        depth_color->g = 0.18;
        depth_color->b = 0.15;
        lighting_depth = 4.0;
        scaling = 1.5;
        waves_height = 1.0;
        detail_height = 0.06;
        turbulence = 0.3;
        foam_coverage = 0.4;
    }

    depth_color->a = 1.0;
    material->base.a = 1.0;
    material->reflection = 1.0;
    material->shininess = 16.0;
    material->hardness = 0.3;
    foam_material->base = colorToHSL(Color(0.8, 0.8, 0.8, 1.0));
    foam_material->reflection = 0.1;
    foam_material->shininess = 1.5;
    foam_material->hardness = 0.2;

    validate();
}
