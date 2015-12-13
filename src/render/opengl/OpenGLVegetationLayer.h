#ifndef OPENGLVEGETATIONLAYER_H
#define OPENGLVEGETATIONLAYER_H

#include "opengl_global.h"

#include <vector>

namespace paysages {
namespace opengl {

class OPENGLSHARED_EXPORT OpenGLVegetationLayer {
  public:
    OpenGLVegetationLayer(OpenGLVegetation *parent, VegetationLayerDefinition *definition, bool own_instances = true);
    virtual ~OpenGLVegetationLayer();

    inline auto getDefinition() const {
        return definition;
    }
    inline int getInstanceCount() const {
        return (int)instances.size();
    }

    /**
     * Collect instances in a given area, and add them to the array *instances*.
     *
     * The array is not checked for already present instances.
     */
    virtual void produceInstancesInArea(double xmin, double xmax, double zmin, double zmax,
                                        vector<OpenGLVegetationInstance *> *instances) const;

    /**
     * Remove instances outside of a given area.
     */
    virtual void removeInstancesOutsideArea(double xmin, double xmax, double zmin, double zmax,
                                            vector<OpenGLVegetationInstance *> *instances) const;

    /**
     * Update the instances list.
     *
     * This should be called when the camera has moved enough to make a change.
     */
    void updateInstances();

    /**
     * Update the impostor textures.
     *
     * This should be called when the camera has moved enough to make a change.
     */
    void updateImpostor();

    /**
     * Perform maintenance tasks that can be perform in a thread.
     *
     * This will be called from a thread separate from the main GUI thread,
     * so it should not call OpenGL functions.
     */
    void threadedUpdate();

    /**
     * Render the vegetation layer.
     *
     * This is called from the GUI thread.
     */
    void render();

    /**
     * Reset to an initial state.
     *
     * It is only useful (and safe) from unit testing.
     */
    void reset();

    /**
     * Set the current camera in use.
     */
    void setCamera(const CameraDefinition *camera);

  private:
    OpenGLVegetation *parent;
    VegetationLayerDefinition *definition;
    double xmin;
    double xmax;
    double zmin;
    double zmax;
    double range;
    bool own_instances;
    vector<OpenGLVegetationInstance *> instances;
    Mutex *lock_instances;
    OpenGLVegetationImpostor *impostor;
    Vector3 *camera_location;
    bool camera_changed;
};
}
}

#endif // OPENGLVEGETATIONLAYER_H
