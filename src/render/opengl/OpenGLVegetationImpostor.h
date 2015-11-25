#ifndef OPENGLVEGETATIONIMPOSTOR_H
#define OPENGLVEGETATIONIMPOSTOR_H

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
    void render(OpenGLShaderProgram *program, const OpenGLVegetationInstance *instance, int index);

    /**
     * Prepare the texture grid for a given model.
     */
    void prepareTexture(const VegetationModelDefinition &model, const Scenery &environment, bool *interrupt);

  private:
    void setVertex(int i, float x, float y, float z);

  private:
    float *vertices;
    int texture_size;
    bool texture_changed;
    Texture2D *texture;
};
}
}

#endif // OPENGLVEGETATIONIMPOSTOR_H
