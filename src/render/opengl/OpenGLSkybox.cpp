#include "OpenGLSkybox.h"

#include <cmath>
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereModelBruneton.h"
#include "FloatNode.h"

OpenGLSkybox::OpenGLSkybox(OpenGLRenderer *renderer) : OpenGLPart(renderer) {
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
    renderer->getScenery()->getAtmosphere()->propDayTime()->addWatcher(this, true);
    renderer->getScenery()->getAtmosphere()->propHumidity()->addWatcher(this, true);
    renderer->getScenery()->getAtmosphere()->propSunRadius()->addWatcher(this, true);
}

void OpenGLSkybox::update() {
    SoftwareBrunetonAtmosphereRenderer *bruneton =
        (SoftwareBrunetonAtmosphereRenderer *)renderer->getAtmosphereRenderer();
    renderer->getSharedState()->set("transmittanceTexture", bruneton->getModel()->getTextureTransmittance());
    renderer->getSharedState()->set("inscatterTexture", bruneton->getModel()->getTextureInscatter());
}

void OpenGLSkybox::render() {
    program->draw(vertices);
}

void OpenGLSkybox::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath() == "/atmosphere/daytime") {
        Vector3 sun_direction = renderer->getAtmosphereRenderer()->getSunDirection(false);
        renderer->getSharedState()->set("sunDirection", sun_direction);

        Color sun_color = renderer->getScenery()->getAtmosphere()->sun_color;
        renderer->getSharedState()->set("sunColor", sun_color);

        renderer->getSharedState()->set("dayTime", renderer->getScenery()->getAtmosphere()->propDayTime()->getValue());
    } else if (node->getPath() == "/atmosphere/humidity") {
        renderer->getSharedState()->set("atmosphereHumidity",
                                        renderer->getScenery()->getAtmosphere()->propHumidity()->getValue());
    } else if (node->getPath() == "/atmosphere/sun_radius") {
        renderer->getSharedState()->set("sunRadius",
                                        renderer->getScenery()->getAtmosphere()->propSunRadius()->getValue());
    }
}
