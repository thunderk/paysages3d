#include "OpenGLWater.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "WaterRenderer.h"
#include "Scenery.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "FloatNode.h"
#include "FloatDiff.h"
#include "IntNode.h"
#include "Logs.h"

static const string path_height = "/terrain/water_height";
static const string path_reflection = "/water/reflection";
static const string path_model = "/water/model";

OpenGLWater::OpenGLWater(OpenGLRenderer *renderer) : OpenGLPart(renderer, "water") {
    enabled = true;

    program = createShader("water");
    program->addVertexSource("water");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("fadeout");
    program->addFragmentSource("ui");
    program->addFragmentSource("noise");
    program->addFragmentSource("water");

    vertices = createVertexArray(false, true);
    vertices->setVertexCount(4);
    vertices->set(0, Vector3(-1.0f, 0.0f, -1.0f));
    vertices->set(1, Vector3(-1.0f, 0.0f, 1.0f));
    vertices->set(2, Vector3(1.0f, 0.0f, -1.0f));
    vertices->set(3, Vector3(1.0f, 0.0f, 1.0f));
}

OpenGLWater::~OpenGLWater() {
}

void OpenGLWater::initialize() {
    // Watch for definition changes
    Scenery *scenery = renderer->getScenery();
    startWatchingPath(scenery, path_height);
    startWatchingPath(scenery, path_reflection);
    startWatchingPath(scenery, path_model, false);
}

void OpenGLWater::update() {
    OpenGLSharedState *state = renderer->getSharedState();

    WaterDefinition *water = renderer->getScenery()->getWater();
    state->set("waterMaterialColor", *water->material->base);
    state->set("waterMaterialReflection", water->material->reflection);
    state->set("waterMaterialShininess", water->material->shininess);
    state->set("waterMaterialHardness", water->material->hardness);

    state->set("waterNoise", renderer->getWaterRenderer()->getNoise());
}

void OpenGLWater::render() {
    if (enabled) {
        program->draw(vertices);
    }
}

void OpenGLWater::nodeChanged(const DefinitionNode *node, const DefinitionDiff *, const DefinitionNode *) {
    OpenGLSharedState *state = renderer->getSharedState();

    if (node->getPath() == path_height) {
        state->set("waterOffset", renderer->getScenery()->getTerrain()->getWaterOffset());
    } else if (node->getPath() == path_reflection) {
        state->set("waterReflection", renderer->getScenery()->getWater()->propReflection()->getValue());
    } else if (node->getPath() == path_model) {
        update();
    }
}

void OpenGLWater::setEnabled(bool enabled) {
    this->enabled = enabled;
}
