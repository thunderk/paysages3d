#include "OpenGLVegetationImpostor.h"

#include <cassert>
#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
#include "OpenGLVertexArray.h"
#include "OpenGLVegetationInstance.h"
#include "Texture2D.h"
#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "AtmosphereDefinition.h"
#include "GodRaysSampler.h"
#include "VegetationRenderer.h"
#include "VegetationInstance.h"
#include "RayCastingResult.h"
#include "SpaceSegment.h"
#include "Matrix4.h"
#include "LightingManager.h"
#include "CameraDefinition.h"

// Get the rotation matrix for an impostor grid index
static inline Matrix4 matrixForIndex(int index) {
    if (index == 0) {
        return Matrix4::newRotateZ(M_PI_2);
    } else if (index < 6) {
        return Matrix4::newRotateY(M_2PI * to_double(index - 1) * 0.2).mult(Matrix4::newRotateZ(M_PI_4));
    } else {
        return Matrix4::newRotateY(M_2PI * to_double(index - 6) * 0.1);
    }
}

OpenGLVegetationImpostor::OpenGLVegetationImpostor(int partsize) {
    int parts = 4;

    vertices = new OpenGLVertexArray(true, true);
    vertices->setVertexCount(4 * parts * parts);
    texture_size = partsize * parts;
    texture = new Texture2D(texture_size, texture_size);
    texture_changed = false;

    state = new OpenGLSharedState();

    setVertex(0, 0.0f, 0.0f);
    setVertex(1, 0.0f, 1.0f);
    setVertex(2, 1.0f, 0.0f);
    setVertex(3, 1.0f, 1.0f);
}

OpenGLVegetationImpostor::~OpenGLVegetationImpostor() {
    delete vertices;
    delete state;
    delete texture;
}

void OpenGLVegetationImpostor::render(OpenGLShaderProgram *program, const OpenGLVegetationInstance *instance,
                                      const Vector3 &camera_location) {
    if (texture_changed) {
        texture_changed = false;
        state->set("impostorTexture", texture);
    }

    int index = getIndex(camera_location, instance->getBase());
    state->set("offset", instance->getBase());
    state->set("size", 2.0 * instance->getSize());
    program->draw(vertices, state, index * 4, 4);
}

void OpenGLVegetationImpostor::prepareTexture(const VegetationModelDefinition &model, const Scenery &environment,
                                              bool *interrupt) {
    Scenery scenery;
    environment.getAtmosphere()->copy(scenery.getAtmosphere());
    SoftwareRenderer renderer(&scenery);
    // FIXME Self light filtering
    renderer.getLightingManager()->clearFilters();
    renderer.getGodRaysSampler()->setEnabled(false);
    VegetationRenderer *vegetation = renderer.getVegetationRenderer();
    VegetationInstance instance(model, VECTOR_ZERO);

    int parts = 4;
    int partsize = texture_size / parts;
    for (int py = 0; py < parts; py++) {
        for (int px = 0; px < parts; px++) {
            int index = py * parts + px;
            Matrix4 rotation = matrixForIndex(index);

            Vector3 cam(5.0, 0.0, 0.0);
            scenery.getCamera()->setLocation(rotation.multPoint(cam));
            scenery.getCamera()->setTarget(VECTOR_ZERO);
            renderer.prepare();

            int startx = px * partsize;
            int starty = py * partsize;

            for (int x = 0; x < partsize; x++) {
                double dx = to_double(x) / to_double(partsize);
                for (int y = 0; y < partsize; y++) {
                    double dy = to_double(y) / to_double(partsize);

                    Vector3 near(5.0, dy - 0.5, -(dx - 0.5));
                    Vector3 far(-5.0, dy - 0.5, -(dx - 0.5));
                    SpaceSegment segment(rotation.multPoint(near.scale(2.0)).add(VECTOR_UP.scale(0.5)),
                                         rotation.multPoint(far.scale(2.0)).add(VECTOR_UP.scale(0.5)));

                    RayCastingResult result = vegetation->renderInstance(segment, instance, false, true);
                    texture->setPixel(startx + x, starty + y,
                                      result.hit ? result.hit_color.normalized() : COLOR_TRANSPARENT);
                }
            }
        }
    }

    texture_changed = true;
}

int OpenGLVegetationImpostor::getIndex(const Vector3 &camera, const Vector3 &instance) const {
    int result;

    VectorSpherical diff = camera.sub(instance).toSpherical();
    if (diff.theta > 1.0) {
        return 0;
    } else {
        double angle = diff.phi / M_2PI;
        if (diff.theta > 0.4) {
            angle = (angle >= 0.9) ? 0.0 : (angle + 0.1);
            return 1 + trunc_to_int(5.0 * angle);
        } else {
            angle = (angle >= 0.95) ? 0.0 : (angle + 0.05);
            return 6 + trunc_to_int(10.0 * angle);
        }
    }

    assert(result >= 0 and result <= 16);
    return result;
}

void OpenGLVegetationImpostor::setVertex(int i, float u, float v) {
    int parts = 4;
    for (int py = 0; py < parts; py++) {
        for (int px = 0; px < parts; px++) {
            int index = py * parts + px;
            Matrix4 rotation = matrixForIndex(index);

            Vector3 vertex = rotation.multPoint(Vector3(1.0, u, -(v - 0.5)));
            vertices->set(index * 4 + i, vertex, (u + to_double(px)) / to_double(parts),
                          (v + to_double(py)) / to_double(parts));
        }
    }
}
