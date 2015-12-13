#ifndef OPENGLVEGETATION_H
#define OPENGLVEGETATION_H

#include "opengl_global.h"

#include "OpenGLPart.h"
#include "DefinitionWatcher.h"

#include <map>

namespace paysages {
namespace opengl {

class VegetationUpdater;

class OPENGLSHARED_EXPORT OpenGLVegetation : public OpenGLPart, public DefinitionWatcher {
  public:
    OpenGLVegetation(OpenGLRenderer *renderer);
    virtual ~OpenGLVegetation();

    inline int getLayerCount() {
        return layers.size();
    }
    inline OpenGLVegetationLayer *getLayer(int i) {
        return layers[i];
    }
    inline OpenGLShaderProgram *getProgram() {
        return program;
    }

    virtual void initialize() override;
    virtual void update() override;
    virtual void render() override;

    virtual void nodeChanged(const DefinitionNode *node, const DefinitionDiff *diff) override;

    /**
     * Get the currently rendered scenery.
     */
    Scenery *getScenery() const;

    /**
     * Call this when the dynamic camera (not the scenery one) changed.
     */
    void cameraChanged(const CameraDefinition *camera);

    /**
     * Acquire the current layers for processing.
     *
     * Don't forget to call releaseLayers once done with them.
     *
     * This will not hold a lock on them, but increment a reference counter to not delete them while in use.
     */
    void acquireLayers(vector<OpenGLVegetationLayer *> &layers);

    /**
     * Release the layers acquired by acquireLayers.
     */
    void releaseLayers(const vector<OpenGLVegetationLayer *> &layers);

    /**
     * Find a rendering layer, by the matching definition layer.
     */
    OpenGLVegetationLayer *findLayer(VegetationLayerDefinition *layer);

    /**
     * Enable or disable the whole vegetation rendering.
     */
    void setEnabled(bool enabled);

    /**
     * Update the *layers* member from the scenery.
     *
     * This will create missing layers, and mark extraneous ones for deletion.
     * This will not update existing layers (they should update themselves by watching their definition).
     */
    void updateLayers();

  private:
    OpenGLShaderProgram *program;
    bool enabled;
    vector<OpenGLVegetationLayer *> layers;
    Mutex *layers_lock;
    VegetationUpdater *updater;
};
}
}

#endif // OPENGLVEGETATION_H
