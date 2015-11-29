#include "OpenGLVegetation.h"

#include "OpenGLRenderer.h"
#include "OpenGLShaderProgram.h"
#include "OpenGLVegetationLayer.h"
#include "Thread.h"
#include "Mutex.h"
#include "Scenery.h"
#include "VegetationDefinition.h"
#include "VegetationLayerDefinition.h"

class paysages::opengl::VegetationUpdater : public Thread {
  public:
    VegetationUpdater(OpenGLVegetation *vegetation) : vegetation(vegetation) {
        interrupted = false;
    }

    void interrupt(bool wait = true) {
        interrupted = true;
        if (wait) {
            join();
        }
    }

  protected:
    virtual void run() override {
        while (not interrupted) {
            std::vector<OpenGLVegetationLayer *> layers;
            vegetation->acquireLayers(layers);
            for (auto layer : layers) {
                layer->threadedUpdate();
            }
            vegetation->releaseLayers(layers);
            timeSleepMs(100);
        }
    }

  private:
    bool interrupted;
    OpenGLVegetation *vegetation;
};

OpenGLVegetation::OpenGLVegetation(OpenGLRenderer *renderer) : OpenGLPart(renderer) {
    layers_lock = new Mutex();
    updater = new VegetationUpdater(this);
    enabled = true;

    // Watch scenery changes
    renderer->getScenery()->getVegetation()->addWatcher(this, true);
}

OpenGLVegetation::~OpenGLVegetation() {
    for (auto layer : layers) {
        delete layer;
    }
    layers.clear();

    delete layers_lock;

    updater->interrupt();
    delete updater;
}

void OpenGLVegetation::initialize() {
    // Start the threaded updater
    updater->start();

    // Prepare shader programs
    program = createShader("vegetation");
    program->addVertexSource("vegetation");
    program->addFragmentSource("atmosphere");
    program->addFragmentSource("tonemapping");
    program->addFragmentSource("ui");
    program->addFragmentSource("vegetation");
}

void OpenGLVegetation::update() {
}

void OpenGLVegetation::render() {
    if (enabled) {
        std::vector<OpenGLVegetationLayer *> layers;
        acquireLayers(layers);
        for (auto layer : layers) {
            layer->render();
        }
        releaseLayers(layers);
    }
}

void OpenGLVegetation::nodeChanged(const DefinitionNode *node, const DefinitionDiff *) {
    if (node->getPath() == "/vegetation") {
        updateLayers();
    }
}

Scenery *OpenGLVegetation::getScenery() const {
    return renderer->getScenery();
}

void OpenGLVegetation::cameraChanged(const CameraDefinition *camera) {
    std::vector<OpenGLVegetationLayer *> layers;
    acquireLayers(layers);
    for (auto layer : layers) {
        layer->setCamera(camera);
    }
    releaseLayers(layers);
}

void OpenGLVegetation::acquireLayers(std::vector<OpenGLVegetationLayer *> &layers) {
    layers_lock->acquire();

    for (auto layer : this->layers) {
        // TODO Reference count
        layers.push_back(layer);
    }

    layers_lock->release();
}

void OpenGLVegetation::releaseLayers(const std::vector<OpenGLVegetationLayer *> &layers) {
    // TODO Reference count
}

OpenGLVegetationLayer *OpenGLVegetation::findLayer(VegetationLayerDefinition *layer) {
    for (auto &l : layers) {
        if (l->getDefinition() == layer) {
            return l;
        }
    }
    return NULL;
}

void OpenGLVegetation::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void OpenGLVegetation::updateLayers() {
    layers_lock->acquire();

    // Add missing layers
    int n = renderer->getScenery()->getVegetation()->getLayerCount();
    for (int i = 0; i < n; i++) {
        VegetationLayerDefinition *layer = renderer->getScenery()->getVegetation()->getVegetationLayer(i);
        if (!findLayer(layer)) {
            layers.push_back(new OpenGLVegetationLayer(this, layer));
        }
    }

    // TODO Mark extraneous layers for deletion

    layers_lock->release();
}
