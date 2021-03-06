#pragma once

#include "opengl_global.h"

namespace paysages {
namespace opengl {

/**
 * A tool to render an "impostor" of a vegetation layer.
 */
class OPENGLSHARED_EXPORT OpenGLVegetationImpostor {
  public:
    OpenGLVegetationImpostor(int partsize = 64);
    ~OpenGLVegetationImpostor();

    inline const Texture2D *getTexture() const {
        return texture;
    }

    /**
     * Render a single instance using this impostor.
     */
    void render(OpenGLShaderProgram *program, const OpenGLVegetationInstance *instance, const Vector3 &camera_location);

    /**
     * Prepare the texture grid for a given model.
     */
    void prepareTexture(const VegetationModelDefinition &model, const Scenery &environment, bool *interrupt);

    /**
     * Get the impostor grid index for an instance, to face the camera.
     */
    int getIndex(const Vector3 &camera, const Vector3 &instance) const;

  private:
    void setVertex(int i, float u, float v);

  private:
    OpenGLVertexArray *vertices;
    OpenGLSharedState *state;
    int texture_size;
    bool texture_changed;
    Texture2D *texture;
};
}
}
