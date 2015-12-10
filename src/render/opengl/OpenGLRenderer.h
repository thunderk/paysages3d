#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "opengl_global.h"

#include "SoftwareRenderer.h"

class QMatrix4x4;

namespace paysages {
namespace opengl {

/**
 * Scenery renderer in an OpenGL context.
 */
class OPENGLSHARED_EXPORT OpenGLRenderer : public SoftwareRenderer {
  public:
    OpenGLRenderer(Scenery *scenery);
    virtual ~OpenGLRenderer();

    inline OpenGLSkybox *getSkybox() const {
        return skybox;
    }
    inline OpenGLWater *getWater() const {
        return water;
    }
    inline OpenGLTerrain *getTerrain() const {
        return terrain;
    }
    inline bool isDisplayed() const {
        return displayed;
    }
    inline bool isStopping() const {
        return stopping;
    }
    inline bool isStopped() const {
        return stopped;
    }

    /**
     * Check for errors in OpenGL context.
     *
     * Will write the error on standard error output, with the *domain* specified.
     */
    void checkForErrors(const std::string &domain);

    /**
     * Release any allocated resource in the opengl context.
     *
     * Must be called in the opengl rendering thread, and before the destructor is called.
     */
    void destroy();

    void initialize();
    void prepareOpenGLState(bool clear = true);
    void resize(int width, int height);
    void paint(bool clear = true);

    /**
     * Ask for the rendering to stop gracefully.
     *
     * Returns true if the rendering is stopped and resources freed.
     *
     * This should be called in an idle loop, while it returns false.
     */
    bool stop();

    /**
     * Reset the whole state (when the scenery has been massively updated).
     */
    void reset();

    /**
     * Pause the rendering process.
     *
     * This will prevent paintings and stop background tasks, until resume() is called.
     */
    void pause();
    /**
     * Resume the rendering process, put on hold by pause().
     */
    void resume();

    /**
     * Set the current mouse location, for use by getMouseProjection().
     */
    void setMouseLocation(int x, int y);

    /**
     * Get the coordinates of the mouse, projected in world space.
     */
    const Vector3 &getMouseProjection();

    /**
     * Change the camera location.
     */
    void setCamera(CameraDefinition *camera);
    void cameraChangeEvent(CameraDefinition *camera);

    inline OpenGLFunctions *getOpenGlFunctions() const {
        return functions;
    }
    inline OpenGLSharedState *getSharedState() const {
        return shared_state;
    }

    virtual double getPrecision(const Vector3 &location) override;
    virtual Color applyMediumTraversal(const Vector3 &location, const Color &color) override;

  private:
    /**
     * Update the mouse_projected member.
     */
    void updateMouseProjection();

  private:
    bool ready;
    bool paused;
    bool displayed;
    bool stopping;
    bool stopped;
    int vp_width;
    int vp_height;

    bool mouse_tracking;
    int mouse_x;
    int mouse_y;
    Vector3 *mouse_projected;

    QMatrix4x4 *view_matrix;

    OpenGLFunctions *functions;
    OpenGLSharedState *shared_state;

    OpenGLSkybox *skybox;
    OpenGLWater *water;
    OpenGLTerrain *terrain;
};
}
}

#endif // OPENGLRENDERER_H
