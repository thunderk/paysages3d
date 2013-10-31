#include "Layers_Test.h"

#include "Layers.h"

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

CPPUNIT_TEST_SUITE_REGISTRATION(Layers_Test);
