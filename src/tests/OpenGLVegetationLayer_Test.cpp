#include "OpenGLVegetationLayer.h"
#include "BaseTestCase.h"

#include "CameraDefinition.h"
#include "OpenGLVegetationInstance.h"
#include "VegetationInstance.h"
#include "VegetationLayerDefinition.h"
#include "VegetationModelDefinition.h"

class FakeLayerRenderer : public OpenGLVegetationLayer {
  public:
    FakeLayerRenderer(VegetationLayerDefinition *definition) : OpenGLVegetationLayer(NULL, definition, false) {
    }
    virtual ~FakeLayerRenderer() {
        for (auto instance : static_instances) {
            delete instance;
        }
    }
    virtual void produceInstancesInArea(double xmin, double xmax, double zmin, double zmax,
                                        vector<OpenGLVegetationInstance *> *instances) const override {
        for (auto instance : static_instances) {
            Vector3 location = instance->getBase();
            if (location.x >= xmin and location.z >= zmin and location.x < xmax and location.z < zmax) {
                instances->push_back(instance);
            }
        }
    }
    vector<OpenGLVegetationInstance *> static_instances;
};

TEST(OpenGLVegetationLayer, updateInstances) {
    CameraDefinition camera;
    VegetationLayerDefinition definition(NULL, "test");
    FakeLayerRenderer rendering(&definition);
    VegetationModelDefinition model(NULL);

    EXPECT_EQ(0, rendering.getInstanceCount());

    rendering.updateInstances();
    EXPECT_EQ(0, rendering.getInstanceCount());

    rendering.static_instances.push_back(
        new OpenGLVegetationInstance(VegetationInstance(model, Vector3(0.0, 0.0, 0.0))));
    rendering.reset();
    rendering.updateInstances();
    EXPECT_EQ(1, rendering.getInstanceCount());

    camera.setLocation(Vector3(-5.0, 0.0, 0.0));
    rendering.setCamera(&camera);
    rendering.updateInstances();
    EXPECT_EQ(1, rendering.getInstanceCount());

    camera.setLocation(Vector3(-11.0, 0.0, 0.0));
    rendering.setCamera(&camera);
    rendering.updateInstances();
    EXPECT_EQ(0, rendering.getInstanceCount());

    camera.setLocation(Vector3(0.0, 0.0, 5.0));
    rendering.setCamera(&camera);
    rendering.updateInstances();
    EXPECT_EQ(1, rendering.getInstanceCount());

    camera.setLocation(Vector3(0.0, 0.0, 15.0));
    rendering.setCamera(&camera);
    rendering.updateInstances();
    EXPECT_EQ(0, rendering.getInstanceCount());
}
