#include "CloudModelCirrus.h"

#include "NoiseGenerator.h"
#include "Vector3.h"
#include "CloudLayerDefinition.h"

CloudModelCirrus::CloudModelCirrus(CloudLayerDefinition *layer) : BaseCloudsModel(layer) {
    noise = new NoiseGenerator();
}

CloudModelCirrus::~CloudModelCirrus() {
    delete noise;
}

void CloudModelCirrus::update() {
    noise->clearLevels();
    noise->addLevelSimple(1.0, -1.0, 1.0);
    noise->addLevelSimple(1.0 / 6.0, -0.6, 0.6);
    noise->addLevelSimple(1.0 / 10.0, -0.15, 0.15);
    noise->addLevelSimple(1.0 / 20.0, -0.09, 0.09);
    noise->addLevelSimple(1.0 / 40.0, -0.06, 0.06);
    noise->addLevelSimple(1.0 / 120.0, -0.03, 0.03);
    noise->addLevelSimple(1.0 / 300.0, -0.01, 0.01);
    noise->normalizeAmplitude(-4.0, 3.0, 0);
    noise->setState(layer->getNoiseState());
}

void CloudModelCirrus::getAltitudeRange(double *min_altitude, double *max_altitude) const {
    *min_altitude = 45.0 + 20.0 * layer->altitude;
    *max_altitude = *min_altitude + 20.0 * layer->scaling;
}

double CloudModelCirrus::getDensity(const Vector3 &location) const {
    double val;
    double min_altitude, max_altitude;
    double noise_scaling = 30.0 * layer->scaling;

    getAltitudeRange(&min_altitude, &max_altitude);

    if (location.y < min_altitude || location.y > max_altitude) {
        return 0.0;
    } else {
        double x = 0.03 * location.x / noise_scaling;
        double y = (location.y - min_altitude) / noise_scaling;
        double z = 0.03 * location.z / noise_scaling;

        // double coverage = layer->coverage * layer->_coverage_by_altitude->getValue((position.y - layer->altitude) /
        // layer->scaling);
        double coverage = layer->coverage;

        val = 0.6 * noise->get3DTotal(x, y, z);
        return val - 1.1 + coverage;
    }
}
