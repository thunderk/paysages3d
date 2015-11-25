#include "OpenGLVegetationLayer.h"

#include OPENGL_FUNCTIONS_INCLUDE
#include <algorithm>
#include "Vector3.h"
#include "CameraDefinition.h"
#include "Mutex.h"
#include "OpenGLVegetation.h"
#include "OpenGLVegetationInstance.h"
#include "OpenGLVegetationImpostor.h"
#include "VegetationLayerDefinition.h"
#include "VegetationPresenceDefinition.h"

OpenGLVegetationLayer::OpenGLVegetationLayer(OpenGLVegetation *parent, VegetationLayerDefinition *definition,
                                             bool own_instances)
    : parent(parent), definition(definition), own_instances(own_instances) {
    lock_instances = new Mutex;
    camera_location = new Vector3(0.0, 0.0, 0.0);
    impostor = new OpenGLVegetationImpostor();
    range = 10.0;

    reset();
}

OpenGLVegetationLayer::~OpenGLVegetationLayer() {
    delete camera_location;
    delete lock_instances;
    delete impostor;
}

void OpenGLVegetationLayer::produceInstancesInArea(double xmin, double xmax, double zmin, double zmax,
                                                   std::vector<OpenGLVegetationInstance *> *instances) const {
    std::vector<VegetationInstance> result;
    definition->getPresence()->collectInstances(&result, *definition->getModel(), xmin, zmin, xmax, zmax, false);
    for (auto raw_instance : result) {
        instances->push_back(new OpenGLVegetationInstance(raw_instance));
    }
}

static bool isNull(void *ptr) {
    return ptr == NULL;
}

static bool compareInstances(OpenGLVegetationInstance *instance1, OpenGLVegetationInstance *instance2) {
    return instance1->getDistance() > instance2->getDistance();
}

void OpenGLVegetationLayer::removeInstancesOutsideArea(double xmin, double xmax, double zmin, double zmax,
                                                       std::vector<OpenGLVegetationInstance *> *instances) const {
    for (auto &instance : *instances) {
        Vector3 base = instance->getBase();
        if (base.x < xmin or base.x >= xmax or base.z < zmin or base.z >= zmax) {
            if (own_instances) {
                delete instance;
            }
            instance = NULL;
        }
    }
    instances->erase(std::remove_if(instances->begin(), instances->end(), isNull), instances->end());
}

void OpenGLVegetationLayer::threadedUpdate() {
    if (camera_changed) {
        camera_changed = false;

        // Compute new area around camera
        double newxmin, newxmax, newzmin, newzmax;
        newxmin = camera_location->x - range;
        newxmax = camera_location->x + range;
        newzmin = camera_location->z - range;
        newzmax = camera_location->z + range;

        // Prepare instances where area grew
        std::vector<OpenGLVegetationInstance *> new_instances;
        if (newxmin < xmin) {
            produceInstancesInArea(newxmin, xmin, newzmin, newzmax, &new_instances);
        }
        if (newxmax > xmax) {
            produceInstancesInArea(xmax, newxmax, newzmin, newzmax, &new_instances);
        }
        if (newzmin < zmin) {
            produceInstancesInArea(xmin, xmax, newzmin, zmin, &new_instances);
        }
        if (newzmax > zmax) {
            produceInstancesInArea(xmin, xmax, zmax, newzmax, &new_instances);
        }

        // Apply the changes
        lock_instances->acquire();
        xmin = newxmin;
        xmax = newxmax;
        zmin = newzmin;
        zmax = newzmax;
        removeInstancesOutsideArea(xmin, xmax, zmin, zmax, &instances);
        instances.insert(instances.end(), new_instances.begin(), new_instances.end());
        for (auto instance: instances) {
            instance->setDistance(instance->getBase().sub(*camera_location).getNorm());
        }
        std::sort(instances.begin(), instances.end(), compareInstances);
        lock_instances->release();

        // Update impostor texture
        bool interrupted = false;
        impostor->prepareTexture(*definition->getModel(), *parent->getScenery(), &interrupted);
    }
}

void OpenGLVegetationLayer::render() {
    lock_instances->acquire();

    // TODO Instanced rendering
    int index = 0;
    for (auto instance : instances) {
        impostor->render(parent->getProgram(), instance, index++);
    }

    lock_instances->release();
}

void OpenGLVegetationLayer::reset() {
    lock_instances->acquire();
    camera_changed = true;
    xmin = 0.0;
    xmax = 0.0;
    zmin = 0.0;
    zmax = 0.0;
    if (own_instances) {
        for (auto instance : instances) {
            delete instance;
        }
    }
    instances.clear();
    lock_instances->release();
}

void OpenGLVegetationLayer::setCamera(const CameraDefinition *camera) {
    if (camera_location->sub(camera->getLocation()).getNorm() > 1.0) {
        *camera_location = camera->getLocation();
        camera_changed = true;
    }
}
