#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "opengl_global.h"

#include <cassert>
#include <QOpenGLShaderProgram>
#include "OpenGLFunctions.h"

namespace paysages {
namespace opengl {

/*!
 * Wrapper for OpenGL vertex arrays.
 */
template <typename Vertex> class VertexArray {
  public:
    VertexArray() {
        ready = false;
        changed = false;
        vertex_count = 1;
        vertices = new Vertex[1];
        index_count = 1;
        indices = new unsigned short[1];
    }

    ~VertexArray() {
        delete[] vertices;
        delete[] indices;
    }

    inline int getVertexCount() {
        return vertex_count;
    }
    inline int getIndexCount() {
        return index_count;
    }
    inline bool isReady() {
        return ready;
    }
    inline bool isChanged() {
        return changed;
    }

    void setVertexCount(int count) {
        assert(count > 0 and count <= 16384);

        delete[] vertices;
        vertices = new Vertex[count];

        vertex_count = count;
    }

    void setGridSize(int edge_vertex_count) {
        assert(edge_vertex_count >= 2);

        setVertexCount(edge_vertex_count * edge_vertex_count);
    }

    void setVertex(int position, const Vertex &vertex) {
        assert(position >= 0 and position < vertex_count);

        vertices[position] = vertex;

        changed = true;
    }

    void setGridVertex(int edge_vertex_count, int x, int y, const Vertex &vertex) {
        setVertex(y * edge_vertex_count + x, vertex);
    }

    void setAutoGridIndices(int edge_vertex_count, int stride = 1) {
        assert(stride >= 1);

        delete[] indices;
        int cell_count = edge_vertex_count - 1;

        index_count = (cell_count / stride) * (cell_count / stride) * 6;
        indices = new unsigned short[index_count];

        int idx = 0;
        for (int y = 0; y < cell_count; y += stride) {
            for (int x = 0; x < cell_count; x += stride) {
                int base = y * edge_vertex_count + x;
                indices[idx++] = base;
                indices[idx++] = base + edge_vertex_count * stride;
                indices[idx++] = base + stride;
                indices[idx++] = base + stride;
                indices[idx++] = base + edge_vertex_count * stride;
                indices[idx++] = base + edge_vertex_count * stride + stride;
            }
        }
    }

    Vertex getVertex(int position) {
        assert(position >= 0 and position < vertex_count);

        return vertices[position];
    }

    Vertex getVertexByIndex(unsigned short index) {
        assert(index >= 0 and index < index_count);

        return getVertex(indices[index]);
    }

    Vertex getGridVertex(int edge_vertex_count, int x, int y) {
        return getVertex(y * edge_vertex_count + x);
    }

    unsigned short getIndex(int position) {
        assert(position >= 0 and position < index_count);

        return indices[position];
    }

    void render(QOpenGLShaderProgram *program, OpenGLFunctions *functions) {
        size_t ptr = (size_t)vertices;

        GLuint vertex = program->attributeLocation("vertex");
        program->setAttributeArray(vertex, GL_FLOAT, (void *)(ptr + offsetof(Vertex, location)), 3, sizeof(Vertex));
        program->enableAttributeArray(vertex);

        GLuint uv = program->attributeLocation("uv");
        program->setAttributeArray(uv, GL_FLOAT, (void *)(ptr + offsetof(Vertex, uv)), 2, sizeof(Vertex));
        program->enableAttributeArray(uv);

        functions->glDrawRangeElements(GL_TRIANGLES, 0, vertex_count - 1, index_count, GL_UNSIGNED_SHORT, indices);

        program->disableAttributeArray(vertex);
        program->disableAttributeArray(uv);
    }

  private:
    bool ready;
    bool changed;

    int vertex_count;
    Vertex *vertices;

    int index_count;
    unsigned short *indices;
};
}
}

#endif // VERTEXARRAY_H
