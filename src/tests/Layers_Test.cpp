#include "Layers_Test.h"
CPPUNIT_TEST_SUITE_REGISTRATION(Layers_Test);

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

void Layers_Test::testConstructor()
{
    Layers layers1(NULL, _construc1);
    CPPUNIT_ASSERT_EQUAL(0, layers1.count());
}

void Layers_Test::testNullLayer()
{
    Layers layers1(NULL, _construc1);

    for (int i = -2; i < 5; i++)
    {
        BaseDefinition* layer = layers1.getLayer(0);
        CPPUNIT_ASSERT(layer != NULL);

        CPPUNIT_ASSERT(NULL == layer->getParent());
        CPPUNIT_ASSERT(layer == layer->getRoot());

        CPPUNIT_ASSERT(QString() == layer->getName());
    }

    CPPUNIT_ASSERT_EQUAL(0, layers1.count());
}

void Layers_Test::testCopy()
{
    Layers layers1(NULL, _construc2);
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(1, layers1.count());

    // Copy with the same constructor
    Layers layers2(NULL, _construc2);
    CPPUNIT_ASSERT_EQUAL(0, layers2.count());
    layers1.copy(&layers2);
    CPPUNIT_ASSERT_EQUAL(1, layers2.count());
    CPPUNIT_ASSERT(QString("test") == layers1.getLayer(0)->getName());
    CPPUNIT_ASSERT(&layers1 == layers1.getLayer(0)->getParent());
    CPPUNIT_ASSERT(QString("test") == layers2.getLayer(0)->getName());
    CPPUNIT_ASSERT(&layers2 == layers2.getLayer(0)->getParent());
    CPPUNIT_ASSERT(layers1.getLayer(0) != layers2.getLayer(0));
}

void Layers_Test::testMaxLayerCount()
{
    Layers layers1(NULL, _construc1);
    layers1.setMaxLayerCount(2);

    CPPUNIT_ASSERT_EQUAL(0, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(1, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(2, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(2, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(2, layers1.count());
    layers1.removeLayer(0);
    CPPUNIT_ASSERT_EQUAL(1, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(2, layers1.count());
    layers1.addLayer();
    CPPUNIT_ASSERT_EQUAL(2, layers1.count());
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

void Layers_Test::testLegacyLayers()
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
    CPPUNIT_ASSERT_EQUAL(0, layersCount(layers1));
    CPPUNIT_ASSERT_EQUAL(1, _legacy_instance_count);
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    CPPUNIT_ASSERT_EQUAL(2, data1->a);
    data1->a = 3;
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    CPPUNIT_ASSERT_EQUAL(3, data1->a);

    // Add an empty layer
    layersAddLayer(layers1, NULL);
    CPPUNIT_ASSERT_EQUAL(1, layersCount(layers1));
    data1 = (LegacyData*)layersGetLayer(layers1, 0);
    CPPUNIT_ASSERT_EQUAL(2, data1->a);
    CPPUNIT_ASSERT_EQUAL(2, _legacy_instance_count);

    // Validation
    data1->a = 6;
    layersValidate(layers1);
    CPPUNIT_ASSERT_EQUAL(5, data1->a);

    // Naming
    layersSetName(layers1, 0, "test1");
    CPPUNIT_ASSERT(QString("test1") == QString(layersGetName(layers1, 0)));

    // Deletion
    delete layers1;
    CPPUNIT_ASSERT_EQUAL(0, _legacy_instance_count);
}
