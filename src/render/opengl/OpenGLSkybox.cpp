#include "OpenGLSkybox.h"

#include <cmath>
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereModelBruneton.h"
#include "FloatNode.h"

OpenGLSkybox::OpenGLSkybox(OpenGLRenderer* renderer):
    OpenGLPart(renderer)
{
    vertices = new float[14 * 3];
}

OpenGLSkybox::~OpenGLSkybox()
{
    delete[] vertices;
}

void OpenGLSkybox::initialize()
{
    program = createShader("skybox");
    program->addVertexSource("skybox");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("skybox");

    setVertex(0, 1.0f, 1.0f, 1.0f);
    setVertex(12, 1.0f, 1.0f, 1.0f);

    setVertex(1, 1.0f, -1.0f, 1.0f);
    setVertex(5, 1.0f, -1.0f, 1.0f);
    setVertex(13, 1.0f, -1.0f, 1.0f);

    setVertex(2, -1.0f, 1.0f, 1.0f);
    setVertex(10, -1.0f, 1.0f, 1.0f);

    setVertex(3, -1.0f, -1.0f, 1.0f);

    setVertex(4, -1.0f, -1.0f, -1.0f);
    setVertex(8, -1.0f, -1.0f, -1.0f);

    setVertex(6, 1.0f, -1.0f, -1.0f);

    setVertex(7, 1.0f, 1.0f, -1.0f);
    setVertex(11, 1.0f, 1.0f, -1.0f);

    setVertex(9, -1.0f, 1.0f, -1.0f);

    // Watch for definition changes
    renderer->getScenery()->getAtmosphere()->propDayTime()->addWatcher(this, true);
    renderer->getScenery()->getAtmosphere()->propHumidity()->addWatcher(this, true);
}

void OpenGLSkybox::update()
{
    SoftwareBrunetonAtmosphereRenderer* bruneton = (SoftwareBrunetonAtmosphereRenderer*)renderer->getAtmosphereRenderer();
    renderer->getSharedState()->set("transmittanceTexture", bruneton->getModel()->getTextureTransmittance());
    renderer->getSharedState()->set("inscatterTexture", bruneton->getModel()->getTextureInscatter());
}

void OpenGLSkybox::render()
{
    program->drawTriangleStrip(vertices, 14);
}
#include "Logs.h"
void OpenGLSkybox::nodeChanged(const DefinitionNode *node, const DefinitionDiff *)
{
    if (node->getPath() == "/atmosphere/daytime")
    {
        Vector3 sun_direction = renderer->getAtmosphereRenderer()->getSunDirection(false);
        renderer->getSharedState()->set("sunDirection", sun_direction);

        Color sun_color = renderer->getScenery()->getAtmosphere()->sun_color;
        renderer->getSharedState()->set("sunColor", sun_color);

        renderer->getSharedState()->set("dayTime", renderer->getScenery()->getAtmosphere()->propDayTime()->getValue());
    }
    else if (node->getPath() == "/atmosphere/humidity")
    {
        renderer->getSharedState()->set("atmosphereHumidity", renderer->getScenery()->getAtmosphere()->propHumidity()->getValue());
    }
}

void OpenGLSkybox::setVertex(int i, float x, float y, float z)
{
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = y;
    vertices[i * 3 + 2] = z;
}
