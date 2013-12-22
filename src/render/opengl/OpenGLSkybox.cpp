#include "OpenGLSkybox.h"

#include <cmath>
#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "AtmosphereRenderer.h"
#include "AtmosphereModelBruneton.h"

OpenGLSkybox::OpenGLSkybox(OpenGLRenderer* renderer):
    OpenGLPart(renderer)
{
    vertices = new float[14 * 3];
    daytime = renderer->getScenery()->getAtmosphere()->_daytime;
}

OpenGLSkybox::~OpenGLSkybox()
{
    delete[] vertices;
}

void OpenGLSkybox::initialize()
{
    program = createShader("skybox");
    program->addVertexSource("skybox");
    program->addFragmentSource("bruneton");
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
}

void OpenGLSkybox::update()
{
    Vector3 sun_direction = renderer->getAtmosphereRenderer()->getSunDirection();
    renderer->getSharedState()->set("sunDirection", sun_direction);

    Color sun_color = renderer->getScenery()->getAtmosphere()->sun_color;
    renderer->getSharedState()->set("sunColor", sun_color);

    SoftwareBrunetonAtmosphereRenderer* bruneton = (SoftwareBrunetonAtmosphereRenderer*)renderer->getAtmosphereRenderer();
    renderer->getSharedState()->set("transmittanceTexture", bruneton->getModel()->getTextureTransmittance());
    renderer->getSharedState()->set("inscatterTexture", bruneton->getModel()->getTextureInscatter());
}

void OpenGLSkybox::render()
{
    program->drawTriangleStrip(vertices, 14);
}

void OpenGLSkybox::alterDayTime(double delta)
{
#if 0
    Scenery* scenery = renderer->getScenery();
    AtmosphereDefinition* definition = scenery->getAtmosphere()->definition;
    daytime = fmod(daytime + delta * 0.001, 1.0);
    // TEMP
    if (daytime > 0.8)
    {
        daytime -= 0.6;
    }
    if (daytime < 0.2)
    {
        daytime += 0.6;
    }

    definition->hour = (int)(daytime * 24.0);
    definition->minute = (int)((daytime - (((double)definition->hour) / 24.0)) * 1440.0);

    AtmosphereDefinitionClass.validate(definition);

    // TODO Update only the sun
    updateScenery(scenery, true);
#endif
}

void OpenGLSkybox::setVertex(int i, float x, float y, float z)
{
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = y;
    vertices[i * 3 + 2] = z;
}
