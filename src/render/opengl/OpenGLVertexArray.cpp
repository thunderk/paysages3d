#include "OpenGLVertexArray.h"

#include "OpenGLFunctions.h"
#include "Logs.h"
#include "Vector3.h"

OpenGLVertexArray::OpenGLVertexArray(bool has_uv, bool strip) : has_uv(has_uv) {
    if (strip) {
        draw_mode = GL_TRIANGLE_STRIP;
    } else {
        draw_mode = GL_TRIANGLES;
    }

    vao = 0;
    vbo_vertex = 0;
    vbo_uv = 0;

    changed = false;
    vertexcount = 0;
    array_vertex = (float *)malloc(sizeof(float));
    array_uv = (float *)malloc(sizeof(float));
}

OpenGLVertexArray::~OpenGLVertexArray() {
    if (vao || vbo_vertex || vbo_uv) {
        Logs::warning() << "[OpenGL] VertexArray not freed in OpenGL state before destructor called" << std::endl;
    }

    free(array_vertex);
    free(array_uv);
}

void OpenGLVertexArray::destroy(OpenGLFunctions *functions) {
    if (vbo_vertex) {
        functions->glDeleteBuffers(1, &vbo_vertex);
        vbo_vertex = 0;
    }
    if (vbo_uv) {
        functions->glDeleteBuffers(1, &vbo_uv);
        vbo_uv = 0;
    }
    if (vao) {
        functions->glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
}

void OpenGLVertexArray::render(OpenGLFunctions *functions) {
    if (changed) {
        changed = false;
        update(functions);
    }

    if (vertexcount and vao) {
        functions->glBindVertexArray(vao);
        functions->glDrawArrays(draw_mode, 0, vertexcount);
        functions->glBindVertexArray(0);
    }
}

void OpenGLVertexArray::setVertexCount(int count) {
    if (count != vertexcount) {
        vertexcount = count;
        if (count < 1) {
            count = 1;
        }
        array_vertex = (float *)realloc(array_vertex, sizeof(float) * count * 3);
        array_uv = (float *)realloc(array_uv, sizeof(float) * count * 2);
        changed = true;
    }
}

void OpenGLVertexArray::set(int index, const Vector3 &location, double u, double v) {
    if (index >= 0 and index < vertexcount) {
        array_vertex[index * 3] = location.x;
        array_vertex[index * 3 + 1] = location.y;
        array_vertex[index * 3 + 2] = location.z;
        array_uv[index * 2] = u;
        array_uv[index * 2 + 1] = v;
        changed = true;
    } else {
        Logs::error() << "[OpenGL] Setting vertex data outside of array bounds" << std::endl;
    }
}

void OpenGLVertexArray::get(int index, Vector3 *location, double *u, double *v) const {
    if (index >= 0 and index < vertexcount) {
        location->x = array_vertex[index * 3];
        location->y = array_vertex[index * 3 + 1];
        location->z = array_vertex[index * 3 + 2];
        *u = array_uv[index * 2];
        *v = array_uv[index * 2 + 1];
    } else {
        Logs::error() << "[OpenGL] Getting vertex data outside of array bounds" << std::endl;
    }
}

void OpenGLVertexArray::copyTo(OpenGLVertexArray *destination) const {
    destination->setVertexCount(vertexcount);
    if (vertexcount) {
        memcpy(destination->array_vertex, array_vertex, sizeof(float) * vertexcount * 3);
        memcpy(destination->array_uv, array_uv, sizeof(float) * vertexcount * 2);
    }
    destination->changed = true;
}

void OpenGLVertexArray::update(OpenGLFunctions *functions) {
    if (not vao) {
        functions->glGenVertexArrays(1, &vao);
    }
    functions->glBindVertexArray(vao);

    if (not vbo_vertex) {
        functions->glGenBuffers(1, &vbo_vertex);
    }
    functions->glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex);
    functions->glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexcount * 3, array_vertex, GL_STATIC_DRAW);
    functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    functions->glEnableVertexAttribArray(0);

    if (not vbo_uv) {
        functions->glGenBuffers(1, &vbo_uv);
    }
    functions->glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    functions->glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexcount * 2, array_uv, GL_STATIC_DRAW);
    functions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    functions->glEnableVertexAttribArray(1);

    functions->glBindVertexArray(0);
}
