#include "VegetationRasterizer.h"
#include "BaseTestCase.h"

#include "Scenery.h"
#include "SoftwareCanvasRenderer.h"
#include "VegetationDefinition.h"
#include "VegetationLayerDefinition.h"

TEST(VegetationRasterizer, isUseful) {
    Scenery scenery;
    SoftwareCanvasRenderer renderer(&scenery);
    VegetationRasterizer rast(&renderer, renderer.getProgressHelper(), 0);

    EXPECT_FALSE(rast.isUseful());

    VegetationLayerDefinition layer(NULL, "test");
    layer.applyPreset(VegetationLayerDefinition::VEGETATION_BASIC_TREES);
    scenery.getVegetation()->addLayer(layer);

    EXPECT_TRUE(rast.isUseful());
}
