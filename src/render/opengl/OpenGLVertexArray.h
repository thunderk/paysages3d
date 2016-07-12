#pragma once

#include "opengl_global.h"

namespace paysages {
namespace opengl {

/**
 * Vertex arrays storage and binding, to render triangles.
 *
 * This will handle VAOs and VBOs automatically.
 */
class OPENGLSHARED_EXPORT OpenGLVertexArray {
  public:
    OpenGLVertexArray(bool has_uv, bool strip = false);
    ~OpenGLVertexArray();

    inline int getVertexCount() const {
        return vertexcount;
    }

    /**
     * Release any allocated resource in the opengl context.
     *
     * Must be called in the opengl rendering thread, and before the destructor is called.
     */
    void destroy(OpenGLFunctions *functions);

    /**
     * Render this array in current opengl context.
     *
     * Must be called in the opengl rendering thread.
     *
     * A shader program must be bound (and uniforms defined) when calling this.
     */
    void render(OpenGLFunctions *functions, int start = 0, int count = -1);

    /**
     * Set the vertex total count.
     */
    void setVertexCount(int count);

    /**
     * Set vertex data in the array.
     *
     * setVertexCount must have been called before to make room for this vertex.
     */
    void set(int index, const Vector3 &location, double u = 0.0, double v = 0.0);

    /**
     * Retrieve vertex data in the array.
     *
     * This is not optimized, only use for testing.
     */
    void get(int index, Vector3 *location, double *u, double *v) const;

    /**
     * Copy this vertex array to another.
     *
     * This does not check it the arrays have the same config, but they certainly should.
     */
    void copyTo(OpenGLVertexArray *destination) const;

  private:
    /**
     * Update the opengl state.
     *
     * Should only be called when the data changed.
     *
     * Must be called in the opengl rendering thread.
     */
    void update(OpenGLFunctions *functions);

  private:
    // Config
    bool has_uv;
    int draw_mode;

    // OpenGL IDs
    unsigned int vao;
    unsigned int vbo_vertex;
    unsigned int vbo_uv;

    // Data
    bool changed;
    int vertexcount;
    float *array_vertex;
    float *array_uv;
};
}
}
