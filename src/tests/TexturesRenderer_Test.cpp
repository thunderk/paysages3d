#include "TexturesRenderer.h"
#include "BaseTestCase.h"

#include "TextureLayerDefinition.h"
#include "TexturesDefinition.h"
#include "Vector3.h"
#include "Zone.h"

TEST(TexturesRenderer, getLayersPresence) {
    TexturesRenderer renderer;
    TexturesDefinition textures(NULL);
    vector<double> result;

    result = renderer.getLayersPresence(&textures, VECTOR_ZERO, VECTOR_UP);
    ASSERT_EQ(0u, result.size());

    TextureLayerDefinition layer1(NULL, "t1");
    textures.addLayer(layer1);

    result = renderer.getLayersPresence(&textures, VECTOR_ZERO, VECTOR_UP);
    ASSERT_EQ(1u, result.size());
    EXPECT_DOUBLE_EQ(1.0, result[0]);

    TextureLayerDefinition layer2(NULL, "t2");
    layer2.terrain_zone->addHeightRangeQuick(0.8, 0.0, 1.0, 2.0, 3.0);
    textures.addLayer(layer2);

    result = renderer.getLayersPresence(&textures, VECTOR_ZERO, VECTOR_UP);
    ASSERT_EQ(2u, result.size());
    EXPECT_DOUBLE_EQ(1.0, result[0]);
    EXPECT_DOUBLE_EQ(0.0, result[1]);

    result = renderer.getLayersPresence(&textures, VECTOR_UP.scale(0.5), VECTOR_UP);
    ASSERT_EQ(2u, result.size());
    EXPECT_DOUBLE_EQ(1.0, result[0]);
    EXPECT_DOUBLE_EQ(0.4, result[1]);

    result = renderer.getLayersPresence(&textures, VECTOR_UP, VECTOR_UP);
    ASSERT_EQ(2u, result.size());
    EXPECT_DOUBLE_EQ(1.0, result[0]);
    EXPECT_DOUBLE_EQ(0.8, result[1]);
}
