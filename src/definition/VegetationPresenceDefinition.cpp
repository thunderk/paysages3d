#include "VegetationPresenceDefinition.h"

#include "FloatNode.h"
#include "FractalNoise.h"
#include "NoiseNode.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "VegetationInstance.h"
#include "VegetationLayerDefinition.h"
#include "VegetationModelDefinition.h"
#include <cmath>

VegetationPresenceDefinition::VegetationPresenceDefinition(VegetationLayerDefinition *parent)
    : DefinitionNode(parent, "presence") {
    noise = new NoiseNode(this);
    interval = new FloatNode(this, "interval", 0.1);
}

bool VegetationPresenceDefinition::collectInstances(vector<VegetationInstance> *result,
                                                    const VegetationModelDefinition &model, double xmin, double zmin,
                                                    double xmax, double zmax, bool outcomers) const {
    if (outcomers) {
        // Expand the area to include outcoming instances
        double max_radius = getMaxHeight();
        xmin -= max_radius;
        zmin -= max_radius;
        xmax += max_radius;
        zmax += max_radius;
    }

    int added = 0;

    auto generator = noise->getGenerator();
    double interval_value = interval->getValue();

    double xstart = xmin - fmod(xmin, interval_value);
    double zstart = zmin - fmod(zmin, interval_value);
    for (double x = xstart; x < xmax; x += interval_value) {
        for (double z = zstart; z < zmax; z += interval_value) {
            double detail = interval_value * 0.1;
            double noise_presence = generator->get2d(detail, x * 0.1, z * 0.1);
            if (noise_presence > 0.0) {
                double size =
                    0.1 + 0.2 * fabs(generator->get2d(detail, z * 10.0, x * 10.0)) * (noise_presence * 0.5 + 0.5);
                double angle = 3.0 * generator->get2d(detail, -x * 20.0, z * 20.0); // TODO balanced distribution
                double xo = x + fabs(generator->get2d(detail, x * 12.0, -z * 12.0));
                double zo = z + fabs(generator->get2d(detail, -x * 27.0, -z * 27.0));
                if (xo >= xmin and xo < xmax and zo >= zmin and zo < zmax) {
                    double y = getScenery()->getTerrain()->getInterpolatedHeight(xo, zo, true, true);
                    result->push_back(VegetationInstance(model, Vector3(xo, y, zo), size, angle));
                    added++;
                }
            }
        }
    }

    return added > 0;
}

double VegetationPresenceDefinition::getMaxHeight() const {
    return 0.3;
}
