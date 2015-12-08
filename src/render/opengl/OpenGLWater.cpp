#include "OpenGLWater.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "WaterRenderer.h"
#include "Scenery.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "NoiseFunctionSimplex.h"
#include "FloatNode.h"
#include "FloatDiff.h"
#include "IntNode.h"

OpenGLWater::OpenGLWater(OpenGLRenderer *renderer) : OpenGLPart(renderer) {
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
    renderer->getScenery()->getTerrain()->propWaterHeight()->addWatcher(this, true);
    renderer->getScenery()->getWater()->propReflection()->addWatcher(this, true);
    renderer->getScenery()->getWater()->propModel()->addWatcher(this, false);
}

void OpenGLWater::update() {
    WaterDefinition *water = renderer->getScenery()->getWater();
    renderer->getSharedState()->set("waterMaterialColor", *water->material->base);
    renderer->getSharedState()->set("waterMaterialReflection", water->material->reflection);
    renderer->getSharedState()->set("waterMaterialShininess", water->material->shininess);
    renderer->getSharedState()->set("waterMaterialHardness", water->material->hardness);

    renderer->getSharedState()->set("simplexSampler", NoiseFunctionSimplex::getNormalTexture(), true, true);
}

void OpenGLWater::render() {
    if (enabled) {
        program->draw(vertices);
    }
}

void OpenGLWater::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath() == "/terrain/water_height") {
        renderer->getSharedState()->set("waterOffset", renderer->getScenery()->getTerrain()->getWaterOffset());
    } else if (node->getPath() == "/water/reflection") {
        renderer->getSharedState()->set("waterReflection",
                                        renderer->getScenery()->getWater()->propReflection()->getValue());
    } else if (node->getPath() == "/water/model") {
        update();
    }
}

void OpenGLWater::setEnabled(bool enabled) {
    this->enabled = enabled;
}
