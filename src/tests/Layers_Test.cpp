#include "BaseTestCase.h"

#include "Layers.h"
#include "PackStream.h"
#include "DiffManager.h"
#include "IntNode.h"
#include "DefinitionDiff.h"

static DefinitionNode *_construc1(Layers *, const string &name) {
    return new DefinitionNode(NULL, name);
}

static DefinitionNode *_construc2(Layers *parent, const string &name) {
    DefinitionNode *result = new DefinitionNode(parent, name);
    return result;
}

TEST(Layers, Constructor) {
    Layers layers1(NULL, "test", _construc1);
    EXPECT_EQ(0, layers1.getLayerCount());
}

TEST(Layers, NullLayer) {
    Layers layers1(NULL, "test", _construc1);

    for (int i = -2; i < 5; i++) {
        DefinitionNode *layer = layers1.getLayer(0);
        EXPECT_NE((DefinitionNode *)NULL, layer);

        EXPECT_EQ(NULL, layer->getParent());
        EXPECT_EQ(layer, layer->getRoot());

        EXPECT_EQ("#NULL#", layer->getName());
    }

    EXPECT_EQ(0, layers1.getLayerCount());
}

TEST(Layers, copy) {
    Layers layers1(NULL, "test", _construc2);
    layers1.addLayer("test");
    EXPECT_EQ(1, layers1.getLayerCount());

    // Copy with the same constructor
    Layers layers2(NULL, "test", _construc2);
    EXPECT_EQ(0, layers2.getLayerCount());
    layers1.copy(&layers2);
    EXPECT_EQ(1, layers2.getLayerCount());
    EXPECT_EQ("test", layers1.getLayer(0)->getName());
    EXPECT_EQ(&layers1, layers1.getLayer(0)->getParent());
    EXPECT_EQ("test", layers2.getLayer(0)->getName());
    EXPECT_EQ(&layers2, layers2.getLayer(0)->getParent());
    EXPECT_NE(layers1.getLayer(0), layers2.getLayer(0));
}

TEST(Layers, maxLayerCount) {
    Layers layers1(NULL, "test", _construc1);
    layers1.setMaxLayerCount(2);

    EXPECT_EQ(0, layers1.getLayerCount());
    layers1.addLayer("t1");
    EXPECT_EQ(1, layers1.getLayerCount());
    layers1.addLayer("t2");
    EXPECT_EQ(2, layers1.getLayerCount());
    layers1.addLayer("t3");
    EXPECT_EQ(2, layers1.getLayerCount());
    layers1.addLayer("t4");
    EXPECT_EQ(2, layers1.getLayerCount());
    layers1.removeLayer(0);
    EXPECT_EQ(1, layers1.getLayerCount());
    layers1.addLayer("t5");
    EXPECT_EQ(2, layers1.getLayerCount());
    layers1.addLayer("t6");
    EXPECT_EQ(2, layers1.getLayerCount());

    layers1.setMaxLayerCount(1);
    EXPECT_EQ(1, layers1.getLayerCount());
    layers1.setMaxLayerCount(0);
    EXPECT_EQ(0, layers1.getLayerCount());
}

TEST(Layers, saveLoad) {
    PackStream *stream;

    Layers layers1(NULL, "test", _construc1);
    layers1.addLayer("first");
    layers1.addLayer("second");
    ASSERT_EQ(2, layers1.getLayerCount());

    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack", true);
    layers1.save(stream);
    delete stream;

    Layers layers2(NULL, "test", _construc1);

    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack");
    layers2.load(stream);
    delete stream;

    ASSERT_EQ(2, layers2.getLayerCount());
    EXPECT_EQ("first", layers2.getLayer(0)->getName());
    EXPECT_EQ("second", layers2.getLayer(1)->getName());
}

static void checkLayerChild(const Layers &layers, int layer, const string &name, int value) {
    ASSERT_EQ(1, layers.getLayerCount());
    ASSERT_TRUE(layers.getLayer(layer)->findByPath(name));
    ASSERT_EQ("int", layers.getLayer(layer)->findByPath(name)->getTypeName());
    EXPECT_EQ(value, ((IntNode *)layers.getLayer(layer)->findByPath(name))->getValue());
}

class TestLayer : public DefinitionNode {
  public:
    TestLayer(Layers *parent, const string &name) : DefinitionNode(parent, name) {
        val = new IntNode(this, "val", 5);
    }
    IntNode *val;
};

static DefinitionNode *_construc3(Layers *parent, const string &name) {
    return new TestLayer(parent, name);
}

TEST(Layers, undoRedo) {
    Layers layers(NULL, "layers", _construc3);

    EXPECT_EQ(0, layers.getLayerCount());
    EXPECT_EQ(0, layers.getDiffManager()->getDiffCount());

    layers.addLayer("testlayer");

    ASSERT_EQ(1, layers.getLayerCount());

    TestLayer *layer = (TestLayer *)layers.getLayer(0);

    EXPECT_EQ(1, layers.getDiffManager()->getDiffCount());
    checkLayerChild(layers, 0, "val", 5);

    layer->val->setValue(8);

    EXPECT_EQ(2, layers.getDiffManager()->getDiffCount());
    checkLayerChild(layers, 0, "val", 8);

    layers.removeLayer(0);

    EXPECT_EQ(3, layers.getDiffManager()->getDiffCount());
    EXPECT_EQ(0, layers.getLayerCount());

    // Start undoing
    layers.getDiffManager()->undo();
    checkLayerChild(layers, 0, "val", 8);
    layers.getDiffManager()->undo();
    checkLayerChild(layers, 0, "val", 5);
    layers.getDiffManager()->undo();
    EXPECT_EQ(0, layers.getLayerCount());

    EXPECT_EQ(3, layers.getDiffManager()->getDiffCount());

    // Start redoing
    layers.getDiffManager()->redo();
    checkLayerChild(layers, 0, "val", 5);
    layers.getDiffManager()->redo();
    checkLayerChild(layers, 0, "val", 8);
    layers.getDiffManager()->redo();
    EXPECT_EQ(0, layers.getLayerCount());

    EXPECT_EQ(3, layers.getDiffManager()->getDiffCount());

    // TODO Test swapping
}

TEST(Layers, generateInitDiffs) {
    Layers layers(NULL, "layers", _construc1);

    vector<const DefinitionDiff *> diffs;
    layers.generateInitDiffs(&diffs);
    EXPECT_EQ(0, (int)diffs.size());

    layers.addLayer("l1");
    layers.addLayer("l2");
    layers.addLayer("l3");

    diffs.clear();
    layers.generateInitDiffs(&diffs);
    EXPECT_EQ(3, (int)diffs.size());

    Layers layers1(NULL, "layers", _construc1);
    for (auto diff : diffs) {
        layers1.applyDiff(diff);
    }
    ASSERT_EQ(3, layers1.getLayerCount());
    EXPECT_EQ("l1", layers1.getLayer(0)->getName());
    EXPECT_EQ("l2", layers1.getLayer(1)->getName());
    EXPECT_EQ("l3", layers1.getLayer(2)->getName());

    for (auto diff : diffs) {
        delete diff;
    }
}
