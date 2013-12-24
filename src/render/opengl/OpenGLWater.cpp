#include "OpenGLWater.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "WaterRenderer.h"
#include "Scenery.h"
#include "WaterDefinition.h"
#include "SurfaceMaterial.h"

OpenGLWater::OpenGLWater(OpenGLRenderer *renderer):
    OpenGLPart(renderer)
{
    vertices = new float[4 * 3];
}

OpenGLWater::~OpenGLWater()
{
    delete[] vertices;
}

void OpenGLWater::initialize()
{
    program = createShader("water");
    program->addVertexSource("water");
    program->addFragmentSource("bruneton");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("water");

    setVertex(0, -1.0f, 0.0f, -1.0f);
    setVertex(1, -1.0f, 0.0f, 1.0f);
    setVertex(2, 1.0f, 0.0f, -1.0f);
    setVertex(3, 1.0f, 0.0f, 1.0f);
}

void OpenGLWater::update()
{
    double water_height = renderer->getWaterRenderer()->getHeightInfo().max_height;
    renderer->getSharedState()->set("waterHeight", water_height);

    Color water_color = renderer->getScenery()->getWater()->material->_rgb;
    renderer->getSharedState()->set("waterColor", water_color);
}

void OpenGLWater::render()
{
    program->drawTriangleStrip(vertices, 4);
}

void OpenGLWater::setVertex(int i, float x, float y, float z)
{
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = y;
    vertices[i * 3 + 2] = z;
}
