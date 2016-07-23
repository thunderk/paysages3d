#include "TexturesRenderer.h"

#include "FractalNoise.h"
#include "LightStatus.h"
#include "LightingManager.h"
#include "NoiseGenerator.h"
#include "NoiseNode.h"
#include "Scenery.h"
#include "SoftwareRenderer.h"
#include "TextureLayerDefinition.h"
#include "TexturesDefinition.h"
#include "Zone.h"
#include <cassert>
#include <cmath>

TexturesRenderer::TexturesRenderer() {
    setQualityFactor(0.5);
}

TexturesRenderer::~TexturesRenderer() {
}

void TexturesRenderer::setQuality(bool normal5) {
    quality_normal5 = normal5;
}

void TexturesRenderer::setQualityFactor(double factor) {
    setQuality(factor > 0.6);
}

double TexturesRenderer::getMaximalDisplacement(TexturesDefinition *textures) const {
    int i, n;
    double disp = 0.0;
    n = textures->getLayerCount();
    for (i = 0; i < n; i++) {
        TextureLayerDefinition *layer = textures->getTextureLayer(i);
        double layer_disp = layer->getMaximalDisplacement();
        if (layer_disp > 0.0) {
            disp += layer_disp;
        }
    }
    return disp;
}

static Vector3 _getDetailNormal(Vector3 base_location, Vector3 base_normal, TextureLayerDefinition *layer,
                                double precision, bool normal5) {
    Vector3 result;

    /* Find guiding vectors in the appoximated local plane */
    Vector3 dx, dy;
    Vector3 pivot;
    if (base_normal.y > 0.95) {
        pivot = VECTOR_NORTH;
    } else {
        pivot = VECTOR_UP;
    }
    dx = base_normal.crossProduct(pivot).normalize();
    dy = base_normal.crossProduct(dx).normalize();

    /* Apply detail noise locally */
    Vector3 center, north, east, south, west;
    auto detail_noise = layer->propDetailNoise()->getGenerator();
    double detail = precision;
    double offset = precision * 0.1;

    center = base_location.add(base_normal.scale(detail_noise->getTriplanar(detail, base_location, base_normal)));

    east = base_location.add(dx.scale(offset));
    east = east.add(base_normal.scale(detail_noise->getTriplanar(detail, east, base_normal)));

    south = base_location.add(dy.scale(offset));
    south = south.add(base_normal.scale(detail_noise->getTriplanar(detail, south, base_normal)));

    if (normal5) {
        west = base_location.add(dx.scale(-offset));
        west = west.add(base_normal.scale(detail_noise->getTriplanar(detail, west, base_normal)));

        north = base_location.add(dy.scale(-offset));
        north = north.add(base_normal.scale(detail_noise->getTriplanar(detail, north, base_normal)));

        result = center.getNormal5(south, north, east, west);
    } else {
        result = center.getNormal3(south, east);
    }

    if (result.dotProduct(base_normal) < 0.0) {
        result = result.scale(-1.0);
    }
    return result;
}

static inline double _getLayerPresence(TextureLayerDefinition *layer, const Vector3 &location, const Vector3 &normal) {
    return layer->terrain_zone->getValue(location, normal);
}

static inline double _getLayerDisplacement(TextureLayerDefinition *layer, const Vector3 &location,
                                           const Vector3 &normal, double presence) {
    auto noise = layer->propDisplacementNoise()->getGenerator();
    return noise->getTriplanar(0.001, location, normal) * presence;
}

static inline double _getLayerDisplacement(TextureLayerDefinition *layer, const Vector3 &location,
                                           const Vector3 &normal) {
    double presence = _getLayerPresence(layer, location, normal);
    return _getLayerDisplacement(layer, location, normal, presence);
}

Vector3 TexturesRenderer::displaceTerrain(const TexturesDefinition *textures, const Vector3 &location,
                                          const Vector3 &normal) const {
    double offset = 0.0;
    int i, n;

    n = textures->getLayerCount();
    for (i = 0; i < n; i++) {
        TextureLayerDefinition *layer = textures->getTextureLayer(i);

        if (layer->hasDisplacement()) {
            offset += _getLayerDisplacement(layer, location, normal);
        }
    }

    return location.add(normal.scale(offset));
}

vector<double> TexturesRenderer::getLayersPresence(const TexturesDefinition *textures, const Vector3 &location,
                                                   const Vector3 &normal) const {
    int n = textures->getLayerCount();
    vector<double> result;
    for (int i = 0; i < n; i++) {
        TextureLayerDefinition *layer = textures->getTextureLayer(i);
        double presence = _getLayerPresence(layer, location, normal);
        result.push_back(presence);
    }
    return result;
}

vector<Vector3> TexturesRenderer::getLayersDisplacement(const TexturesDefinition *textures, const Vector3 &location,
                                                        const Vector3 &normal, const vector<double> &presence) const {
    int n = textures->getLayerCount();
    assert(presence.size() == to_size(n));
    vector<Vector3> result;
    Vector3 displaced = location;
    for (int i = 0; i < n; i++) {
        double layer_presence = presence[i];
        if (layer_presence > 0.0) {
            TextureLayerDefinition *layer = textures->getTextureLayer(i);
            double displacement = _getLayerDisplacement(layer, location, normal, layer_presence);
            displaced = displaced.add(normal.scale(displacement * layer_presence));
        }
        result.push_back(displaced);
    }
    return result;
}

Color TexturesRenderer::getFinalComposition(const TexturesDefinition *textures, LightingManager *lighting,
                                            const vector<double> &presence, const vector<Vector3> &location,
                                            const vector<Vector3> &normal, double precision, const Vector3 &eye) const {
    unsigned int n = textures->getLayerCount();
    assert(presence.size() == to_size(n));
    assert(location.size() == to_size(n));
    assert(normal.size() == to_size(n));
    Color result = COLOR_BLACK;

    // Start at the top-most covering layer (layers underneath are only important for displacement, not color)
    unsigned int i;
    unsigned int ipos = 0;
    for (i = n - 1; i > 0; i--) {
        if (presence[i] > 0.1 and ipos == 0) {
            ipos = i;
        } else if (presence[i] > 0.99999) {
            break;
        }
    }

    // Prepare the lighing status
    LightStatus status(lighting, location[ipos], eye);
    lighting->fillStatus(status, location[ipos]);

    // Iterate on each layer
    while (i < n) {
        double layer_presence = presence[i];
        if (layer_presence > 0.0) {
            TextureLayerDefinition *layer = textures->getTextureLayer(i);
            auto detail_normal = _getDetailNormal(location[i], normal[i], layer, precision, quality_normal5);
            Color layer_color = status.apply(detail_normal, *layer->material);
            layer_color.a *= layer_presence;
            result.mask(layer_color);
        }
        i++;
    }
    return result;
}
