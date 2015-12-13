#include "BaseTestCase.h"
#include "OpenGLSkybox.h"

#include "Scenery.h"
#include "OpenGLSharedState.h"
#include "OpenGLRenderer.h"

TEST(OpenGLSkybox, glvariable_atmosphereHumidity) {
    Scenery scenery;
    OpenGLRenderer renderer(&scenery);

    OpenGLSkybox glvegetation(&renderer);
    glvegetation.initialize();
    EXPECT_GLVARIABLE_FLOAT(0.0, renderer.getSharedState(), "atmosphereHumidity");

    scenery.set("/atmosphere/humidity", 0.8);
    EXPECT_GLVARIABLE_FLOAT(0.8, renderer.getSharedState(), "atmosphereHumidity");

    scenery.set("/atmosphere/humidity", 0.6);
    EXPECT_GLVARIABLE_FLOAT(0.6, renderer.getSharedState(), "atmosphereHumidity");

    scenery.undo();
    EXPECT_GLVARIABLE_FLOAT(0.8, renderer.getSharedState(), "atmosphereHumidity");

    scenery.redo();
    EXPECT_GLVARIABLE_FLOAT(0.6, renderer.getSharedState(), "atmosphereHumidity");
}
