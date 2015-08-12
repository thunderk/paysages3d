#include "BaseTestCase.h"

#include "Layers.h"
#include "PackStream.h"

DefinitionNode* _construc1(Layers*)
{
    return new DefinitionNode(NULL, "test");
}

DefinitionNode* _construc2(Layers* parent)
{
    DefinitionNode* result = new DefinitionNode(parent, "test");
    return result;
}

TEST(Layers, Constructor)
{
    Layers layers1(NULL, "test", _construc1);
    EXPECT_EQ(0, layers1.count());
}

TEST(Layers, NullLayer)
{
    Layers layers1(NULL, "test", _construc1);

    for (int i = -2; i < 5; i++)
    {
        DefinitionNode* layer = layers1.getLayer(0);
        EXPECT_NE((DefinitionNode*)NULL, layer);

        EXPECT_EQ(NULL, layer->getParent());
        EXPECT_EQ(layer, layer->getRoot());

        EXPECT_EQ("test", layer->getName());
    }

    EXPECT_EQ(0, layers1.count());
}

TEST(Layers, copy)
{
    Layers layers1(NULL, "test", _construc2);
    layers1.addLayer();
    EXPECT_EQ(1, layers1.count());

    // Copy with the same constructor
    Layers layers2(NULL, "test", _construc2);
    EXPECT_EQ(0, layers2.count());
    layers1.copy(&layers2);
    EXPECT_EQ(1, layers2.count());
    EXPECT_EQ("test", layers1.getLayer(0)->getName());
    EXPECT_EQ(&layers1, layers1.getLayer(0)->getParent());
    EXPECT_EQ("test", layers2.getLayer(0)->getName());
    EXPECT_EQ(&layers2, layers2.getLayer(0)->getParent());
    EXPECT_NE(layers1.getLayer(0), layers2.getLayer(0));
}

TEST(Layers, maxLayerCount)
{
    Layers layers1(NULL, "test", _construc1);
    layers1.setMaxLayerCount(2);

    EXPECT_EQ(0, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(1, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(2, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(2, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(2, layers1.count());
    layers1.removeLayer(0);
    EXPECT_EQ(1, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(2, layers1.count());
    layers1.addLayer();
    EXPECT_EQ(2, layers1.count());
}
