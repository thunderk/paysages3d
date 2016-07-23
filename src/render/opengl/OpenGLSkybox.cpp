#include "OpenGLSkybox.h"

#include "AtmosphereDefinition.h"
#include "AtmosphereModelBruneton.h"
#include "AtmosphereRenderer.h"
#include "CelestialBodyDefinition.h"
#include "FloatNode.h"
#include "Logs.h"
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "Scenery.h"
#include <cmath>

static const string path_humidity = "/atmosphere/humidity";
static const string path_sun_phi = "/atmosphere/sun/phi";
static const string path_sun_theta = "/atmosphere/sun/theta";
static const string path_sun_radius = "/atmosphere/sun/radius";

OpenGLSkybox::OpenGLSkybox(OpenGLRenderer *renderer) : OpenGLPart(renderer, "skybox") {
    program = createShader("skybox");
    program->addVertexSource("skybox");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("skybox");

    vertices = createVertexArray(false, true);

    vertices->setVertexCount(14);

    vertices->set(0, Vector3(1.0f, 1.0f, 1.0f));
    vertices->set(12, Vector3(1.0f, 1.0f, 1.0f));

    vertices->set(1, Vector3(1.0f, -1.0f, 1.0f));
    vertices->set(5, Vector3(1.0f, -1.0f, 1.0f));
    vertices->set(13, Vector3(1.0f, -1.0f, 1.0f));

    vertices->set(2, Vector3(-1.0f, 1.0f, 1.0f));
    vertices->set(10, Vector3(-1.0f, 1.0f, 1.0f));

    vertices->set(3, Vector3(-1.0f, -1.0f, 1.0f));

    vertices->set(4, Vector3(-1.0f, -1.0f, -1.0f));
    vertices->set(8, Vector3(-1.0f, -1.0f, -1.0f));

    vertices->set(6, Vector3(1.0f, -1.0f, -1.0f));

    vertices->set(7, Vector3(1.0f, 1.0f, -1.0f));
    vertices->set(11, Vector3(1.0f, 1.0f, -1.0f));

    vertices->set(9, Vector3(-1.0f, 1.0f, -1.0f));
}

OpenGLSkybox::~OpenGLSkybox() {
}

void OpenGLSkybox::initialize() {
    // Watch for definition changes
    Scenery *scenery = renderer->getScenery();
    startWatchingPath(scenery, path_sun_phi);
    startWatchingPath(scenery, path_sun_theta);
    startWatchingPath(scenery, path_sun_radius);
    startWatchingPath(scenery, path_humidity);
}

void OpenGLSkybox::update() {
    Logs::debug("OpenGL") << "Updating atmosphere textures" << endl;

    SoftwareBrunetonAtmosphereRenderer *bruneton =
        (SoftwareBrunetonAtmosphereRenderer *)renderer->getAtmosphereRenderer();
    renderer->getSharedState()->set("transmittanceTexture", bruneton->getModel()->getTextureTransmittance());
    renderer->getSharedState()->set("inscatterTexture", bruneton->getModel()->getTextureInscatter());
    renderer->getSharedState()->set("irradianceTexture", bruneton->getModel()->getTextureIrradiance());
}

void OpenGLSkybox::render() {
    program->draw(vertices);
}

void OpenGLSkybox::nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff, const DefinitionNode *parent) {
    OpenGLSharedState *state = renderer->getSharedState();
    AtmosphereDefinition *newdef = renderer->getScenery()->getAtmosphere();

    if (node->getPath() == path_sun_phi or node->getPath() == path_sun_theta) {
        Vector3 sun_direction = newdef->childSun()->getGlobalDirection();
        state->set("sunDirection", sun_direction);

        Color sun_color = newdef->sun_color;
        state->set("sunColor", sun_color);

        state->set("dayTime", newdef->getDaytime());
    }

    DefinitionWatcher::nodeChanged(node, diff, parent);
}

void OpenGLSkybox::floatNodeChanged(const string &path, double new_value, double) {
    OpenGLSharedState *state = renderer->getSharedState();

    if (path == path_humidity) {
        state->set("atmosphereHumidity", new_value);
    } else if (path == path_sun_radius) {
        state->set("sunRadius", renderer->getScenery()->getAtmosphere()->childSun()->getAngularRadius());
    }
}
