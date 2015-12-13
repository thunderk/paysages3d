#include "BaseTestCase.h"
#include "OpenGLVegetation.h"

#include "Scenery.h"
#include "VegetationDefinition.h"
#include "VegetationLayerDefinition.h"
#include "OpenGLRenderer.h"

TEST(OpenGLVegetation, updateLayers) {
    Scenery scenery;
    OpenGLRenderer renderer(&scenery);
    OpenGLVegetation glvegetation(&renderer);

    EXPECT_EQ(0, glvegetation.getLayerCount());

    scenery.getVegetation()->addLayer("test");

    EXPECT_EQ(1, glvegetation.getLayerCount());
}
