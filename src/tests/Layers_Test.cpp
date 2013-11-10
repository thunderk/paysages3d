#include "BaseTestCase.h"

#include "Layers.h"
#include "PackStream.h"

BaseDefinition* _construc1(Layers*)
{
    return new BaseDefinition(NULL);
}

BaseDefinition* _construc2(Layers* parent)
{
    BaseDefinition* result = new BaseDefinition(parent);
    result->setName("test");
    return result;
}

TEST(Layers, Constructor)
{
    Layers layers1(NULL, _construc1);
    EXPECT_EQ(0, layers1.count());
}

TEST(Layers, NullLayer)
{
    Layers layers1(NULL, _construc1);

    for (int i = -2; i < 5; i++)
    {
        BaseDefinition* layer = layers1.getLayer(0);
        EXPECT_NE((BaseDefinition*)NULL, layer);

        EXPECT_EQ(NULL, layer->getParent());
        EXPECT_EQ(layer, layer->getRoot());

        EXPECT_EQ(QString(), layer->getName());
    }

    EXPECT_EQ(0, layers1.count());
}

TEST(Layers, copy)
{
    Layers layers1(NULL, _construc2);
    layers1.addLayer();
    EXPECT_EQ(1, layers1.count());

    // Copy with the same constructor
    Layers layers2(NULL, _construc2);
    EXPECT_EQ(0, layers2.count());
    layers1.copy(&layers2);
    EXPECT_EQ(1, layers2.count());
    EXPECT_EQ(QString("test"), layers1.getLayer(0)->getName());
    EXPECT_EQ(&layers1, layers1.getLayer(0)->getParent());
    EXPECT_EQ(QString("test"), layers2.getLayer(0)->getName());
    EXPECT_EQ(&layers2, layers2.getLayer(0)->getParent());
    EXPECT_NE(layers1.getLayer(0), layers2.getLayer(0));
}

TEST(Layers, maxLayerCount)
{
    Layers layers1(NULL, _construc1);
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

static int _legacy_instance_count = 0;

typedef struct
{
    int a;
} LegacyData;

static void* _legacy_create()
{
    LegacyData* data = new LegacyData;
    data->a = 2;
    _legacy_instance_count++;
    return data;
}

static void _legacy_delete(void* data)
{
    delete (LegacyData*)data;
    _legacy_instance_count--;
}

static void _legacy_save(PackStream* stream, void* data)
{
    stream->write(&((LegacyData*)data)->a);
}

static void _legacy_load(PackStream* stream, void* data)
{
    stream->read(&((LegacyData*)data)->a);
}

static void _legacy_validate(void* data)
{
    LegacyData* ldata = (LegacyData*)data;
    if (ldata->a > 5)
    {
        ldata->a = 5;
    }
}

static void _legacy_copy(void* source, void* destination)
{
    ((LegacyData*)destination)->a = ((LegacyData*)source)->a;
}

TEST(Layers, LegacyLayers)
{
    LayerType type;
    type.callback_create = _legacy_create;
    type.callback_delete = _legacy_delete;
    type.callback_save = _legacy_save;
    type.callback_load = _legacy_load;
    type.callback_validate = _legacy_validate;
    type.callback_copy = _legacy_copy;

    Layers* layers1 = layersCreate(type, 3);

    LegacyData* data1;

    // Test the null layer
    EXPECT_EQ(0, layersCount(layers1));
    EXPECT_EQ(1, _legacy_instance_count);
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    EXPECT_EQ(2, data1->a);
    data1->a = 3;
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    EXPECT_EQ(3, data1->a);

    // Add an empty layer
    layersAddLayer(layers1, NULL);
    EXPECT_EQ(1, layersCount(layers1));
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    EXPECT_EQ(2, data1->a);
    EXPECT_EQ(2, _legacy_instance_count);

    // Validation
    data1->a = 6;
    layersValidate(layers1);
    EXPECT_EQ(5, data1->a);

    // Naming
    layersSetName(layers1, 0, "test1");
    EXPECT_EQ(QString("test1"), QString(layersGetName(layers1, 0)));

    // Deletion
    delete layers1;
    EXPECT_EQ(0, _legacy_instance_count);
}
