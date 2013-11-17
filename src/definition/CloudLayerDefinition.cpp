#include "CloudLayerDefinition.h"

#include "Curve.h"
#include "NoiseGenerator.h"
#include "SurfaceMaterial.h"
#include "PackStream.h"

CloudLayerDefinition::CloudLayerDefinition(BaseDefinition* parent):
    BaseDefinition(parent)
{
    _coverage_by_altitude = new Curve;
    _coverage_noise = new NoiseGenerator();
    _shape_noise = new NoiseGenerator();
    _edge_noise = new NoiseGenerator();
    material = new SurfaceMaterial;
}

CloudLayerDefinition::~CloudLayerDefinition()
{
    delete _coverage_by_altitude;
    delete _coverage_noise;
    delete _shape_noise;
    delete _edge_noise;
    delete material;
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(const CloudLayerDefinition& other, BaseDefinition* parent)
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    other.copy(layer);
    return layer;
}

CloudLayerDefinition* CloudLayerDefinition::newCopy(BaseDefinition* parent) const
{
    CloudLayerDefinition* layer = new CloudLayerDefinition(parent);
    copy(layer);
    return layer;
}

void CloudLayerDefinition::save(PackStream* stream) const
{
    BaseDefinition::save(stream);

    int clouds_type = (int)type;

    stream->write(&clouds_type);
    stream->write(&lower_altitude);
    stream->write(&thickness);
    _coverage_by_altitude->save(stream);
    _coverage_noise->save(stream);
    _shape_noise->save(stream);
    _edge_noise->save(stream);
    materialSave(stream, material);
    stream->write(&hardness);
    stream->write(&transparencydepth);
    stream->write(&lighttraversal);
    stream->write(&minimumlight);
    stream->write(&shape_scaling);
    stream->write(&edge_scaling);
    stream->write(&edge_length);
    stream->write(&base_coverage);
}

void CloudLayerDefinition::load(PackStream* stream)
{
    BaseDefinition::load(stream);

    int clouds_type;

    stream->read(&clouds_type);
    type = (CloudsType)clouds_type;
    stream->read(&lower_altitude);
    stream->read(&thickness);
    _coverage_by_altitude->load(stream);
    _coverage_noise->load(stream);
    _shape_noise->load(stream);
    _edge_noise->load(stream);
    materialLoad(stream, material);
    stream->read(&hardness);
    stream->read(&transparencydepth);
    stream->read(&lighttraversal);
    stream->read(&minimumlight);
    stream->read(&shape_scaling);
    stream->read(&edge_scaling);
    stream->read(&edge_length);
    stream->read(&base_coverage);

    validate();
}

void CloudLayerDefinition::copy(BaseDefinition* _destination) const
{
    BaseDefinition::copy(_destination);

    CloudLayerDefinition* destination = (CloudLayerDefinition*)_destination;

    destination->type = type;
    destination->lower_altitude = lower_altitude;
    destination->thickness = thickness;
    _coverage_by_altitude->copy(destination->_coverage_by_altitude);
    _coverage_noise->copy(destination->_coverage_noise);
    _shape_noise->copy(destination->_shape_noise);
    _edge_noise->copy(destination->_edge_noise);
    *destination->material = *material;
    destination->hardness = hardness;
    destination->transparencydepth = transparencydepth;
    destination->lighttraversal = lighttraversal;
    destination->minimumlight = minimumlight;
    destination->shape_scaling = shape_scaling;
    destination->edge_scaling = edge_scaling;
    destination->edge_length = edge_length;
    destination->base_coverage = base_coverage;
}

void CloudLayerDefinition::validate()
{
    if (shape_scaling < 0.0001)
    {
        shape_scaling = 0.00001;
    }
    if (edge_scaling < 0.0001)
    {
        edge_scaling = 0.00001;
    }

    _coverage_by_altitude->clear();
    _shape_noise->clearLevels();
    _edge_noise->clearLevels();
    _coverage_noise->clearLevels();

    _coverage_noise->addLevelsSimple(2, 10.0, 0.0, 1.0, 0.0);
    _coverage_noise->addLevelsSimple(2, 1.0, 0.0, 1.0, 0.0);
    _coverage_noise->setFunctionParams(NOISE_FUNCTION_NAIVE, 0.0, 0.0);
    switch (type)
    {
        case CLOUDS_TYPE_CIRRUS:
            _coverage_by_altitude->addPoint(0.0, 0.0);
            _coverage_by_altitude->addPoint(0.5, 1.0);
            _coverage_by_altitude->addPoint(1.0, 0.0);
            _shape_noise->addLevelsSimple(3, 1.0, 0.0, 1.0, 0.5);
            _shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.0, 0.0);
            _edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            _edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.2, 0.0);
            break;
        case CLOUDS_TYPE_CUMULUS:
            _coverage_by_altitude->addPoint(0.0, 0.0);
            _coverage_by_altitude->addPoint(0.1, 1.0);
            _coverage_by_altitude->addPoint(0.4, 0.8);
            _coverage_by_altitude->addPoint(0.7, 1.0);
            _coverage_by_altitude->addPoint(1.0, 0.0);
            _shape_noise->addLevelsSimple(7, 1.0, 0.0, 1.0, 0.5);
            _shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.4, 0.0);
            _edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            _edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.8, 0.0);
            break;
        case CLOUDS_TYPE_STRATOCUMULUS:
            _coverage_by_altitude->addPoint(0.0, 0.0);
            _coverage_by_altitude->addPoint(0.2, 1.0);
            _coverage_by_altitude->addPoint(0.5, 1.0);
            _coverage_by_altitude->addPoint(1.0, 0.0);
            _shape_noise->addLevelsSimple(4, 1.0, 0.0, 1.0, 0.5);
            _shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.3, 0.0);
            _edge_noise->addLevelsSimple(6, 1.0, -0.5, 0.5, 0.5);
            _edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, 0.5, 0.0);
            break;
        case CLOUDS_TYPE_STRATUS:
            _coverage_by_altitude->addPoint(0.0, 0.0);
            _coverage_by_altitude->addPoint(0.2, 1.0);
            _coverage_by_altitude->addPoint(0.8, 1.0);
            _coverage_by_altitude->addPoint(1.0, 0.0);
            _shape_noise->addLevelsSimple(3, 1.0, 0.0, 1.0, 0.5);
            _shape_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.3, 0.0);
            _edge_noise->addLevelsSimple(4, 1.0, -0.5, 0.5, 0.5);
            _edge_noise->setFunctionParams(NOISE_FUNCTION_SIMPLEX, -0.5, 0.0);
            break;
        default:
            break;
    }

    _coverage_noise->normalizeAmplitude(-1.0, 3.0, 0);
    _shape_noise->normalizeAmplitude(-0.5, 0.5, 0);
    _edge_noise->normalizeAmplitude(-0.5, 0.5, 0);

    materialValidate(material);
}

void CloudLayerDefinition::applyPreset(CloudsLayerPreset preset)
{
    _coverage_noise->randomizeOffsets();
    _edge_noise->randomizeOffsets();
    _shape_noise->randomizeOffsets();

    material->base = colorToHSL(colorFromValues(0.7, 0.7, 0.7, 1.0));

    switch (preset)
    {
        case CLOUDS_LAYER_PRESET_CIRRUS:
            type = CLOUDS_TYPE_CIRRUS;
            lower_altitude = 25.0;
            thickness = 2.0;
            material->reflection = 0.4;
            material->shininess = 0.5;
            hardness = 0.0;
            transparencydepth = 3.0;
            lighttraversal = 10.0;
            minimumlight = 0.6;
            shape_scaling = 8.0;
            edge_scaling = 2.0;
            edge_length = 0.8;
            base_coverage = 0.6;
            break;
        case CLOUDS_LAYER_PRESET_CUMULUS:
            type = CLOUDS_TYPE_CUMULUS;
            lower_altitude = 15.0;
            thickness = 15.0;
            material->reflection = 0.5;
            material->shininess = 1.2;
            hardness = 0.25;
            transparencydepth = 1.5;
            lighttraversal = 8.0;
            minimumlight = 0.4;
            shape_scaling = 20.0;
            edge_scaling = 2.0;
            edge_length = 0.0;
            base_coverage = 0.7;
            break;
        case CLOUDS_LAYER_PRESET_STRATOCUMULUS:
            type = CLOUDS_TYPE_STRATOCUMULUS;
            lower_altitude = 5.0;
            thickness = 6.0;
            material->reflection = 0.3;
            material->shininess = 0.8;
            hardness = 0.25;
            transparencydepth = 1.5;
            lighttraversal = 7.0;
            minimumlight = 0.4;
            shape_scaling = 10.0;
            edge_scaling = 0.8;
            edge_length = 0.3;
            base_coverage = 0.4;
            break;
        case CLOUDS_LAYER_PRESET_STRATUS:
            type = CLOUDS_TYPE_STRATUS;
            lower_altitude = 3.0;
            thickness = 4.0;
            material->reflection = 0.1;
            material->shininess = 0.8;
            hardness = 0.1;
            transparencydepth = 3.0;
            lighttraversal = 10.0;
            minimumlight = 0.6;
            shape_scaling = 8.0;
            edge_scaling = 2.0;
            edge_length = 1.0;
            base_coverage = 0.4;
            break;
        default:
            break;
    }

    validate();
}
