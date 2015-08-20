#include "OpenGLWater.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "WaterRenderer.h"
#include "Scenery.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"
#include "NoiseFunctionSimplex.h"
#include "FloatNode.h"
#include "FloatDiff.h"

OpenGLWater::OpenGLWater(OpenGLRenderer *renderer):
    OpenGLPart(renderer)
{
    vertices = new float[4 * 3];
    enabled = true;
}

OpenGLWater::~OpenGLWater()
{
    delete[] vertices;
}

void OpenGLWater::initialize()
{
    program = createShader("water");
    program->addVertexSource("water");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("fadeout");
    program->addFragmentSource("noise");
    program->addFragmentSource("water");

    setVertex(0, -1.0f, 0.0f, -1.0f);
    setVertex(1, -1.0f, 0.0f, 1.0f);
    setVertex(2, 1.0f, 0.0f, -1.0f);
    setVertex(3, 1.0f, 0.0f, 1.0f);

    // Watch for definition changes
    renderer->getScenery()->getTerrain()->propWaterHeight()->addWatcher(this, true);
}

void OpenGLWater::update()
{
    Color water_color = *renderer->getScenery()->getWater()->material->base;
    renderer->getSharedState()->set("waterColor", water_color);

    double water_reflection = renderer->getScenery()->getWater()->reflection;
    renderer->getSharedState()->set("waterReflection", water_reflection);

    renderer->getSharedState()->set("simplexSampler", NoiseFunctionSimplex::getNormalTexture(), true, true);
}

void OpenGLWater::render()
{
    if (enabled)
    {
        program->drawTriangleStrip(vertices, 4);
    }
}

void OpenGLWater::setVertex(int i, float x, float y, float z)
{
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = y;
    vertices[i * 3 + 2] = z;
}

void OpenGLWater::nodeChanged(const DefinitionNode *node, const DefinitionDiff *)
{
    if (node->getPath() == "/terrain/water_height")
    {
        renderer->getSharedState()->set("waterOffset", renderer->getScenery()->getTerrain()->getWaterOffset());
    }
}

void OpenGLWater::setEnabled(bool enabled)
{
    this->enabled = enabled;
}
