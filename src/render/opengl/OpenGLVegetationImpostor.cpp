#include "OpenGLVegetationImpostor.h"

#include "OpenGLShaderProgram.h"
#include "OpenGLSharedState.h"
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

OpenGLVegetationImpostor::OpenGLVegetationImpostor(int partsize) {
    vertices = new float[4 * 3];
    texture_size = partsize * 4;
    texture = new Texture2D(texture_size, texture_size);
    texture_changed = true;

    setVertex(0, 0.0f, 0.0f, 0.0f);
    setVertex(1, 0.0f, 0.0f, 1.0f);
    setVertex(2, 0.0f, 1.0f, 0.0f);
    setVertex(3, 0.0f, 1.0f, 1.0f);
}

OpenGLVegetationImpostor::~OpenGLVegetationImpostor() {
    delete[] vertices;
    delete texture;
}

void OpenGLVegetationImpostor::render(OpenGLShaderProgram *program, const OpenGLVegetationInstance *instance,
                                      int index) {
    if (index == 0 or texture_changed) {
        texture_changed = false;
        program->getState()->set("impostorTexture", texture);
    }
    program->getState()->setInt("index", 15); // TODO
    program->getState()->set("offset", instance->getBase());
    program->getState()->set("size", instance->getSize());
    program->drawTriangleStrip(vertices, 4);
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
    Matrix4 rotation;
    for (int py = 0; py < parts; py++) {
        for (int px = 0; px < parts; px++) {
            int index = py * parts + px;
            if (index == 0) {
                rotation = Matrix4::newRotateX(-M_PI_2);
            } else if (index < 6) {
                rotation = Matrix4::newRotateY(M_2PI * (double)(index - 1) * 0.2).mult(Matrix4::newRotateX(-M_PI_4));
            } else {
                rotation = Matrix4::newRotateY(M_2PI * (double)(index - 6) * 0.1);
            }

            Vector3 cam(0.0, 0.0, 5.0);
            scenery.getCamera()->setLocation(rotation.multPoint(cam));
            scenery.getCamera()->setTarget(VECTOR_ZERO);
            renderer.prepare();

            int startx = px * partsize;
            int starty = py * partsize;

            for (int x = 0; x < partsize; x++) {
                double dx = (double)x / (double)partsize;
                for (int y = 0; y < partsize; y++) {
                    double dy = (double)y / (double)partsize;

                    Vector3 near(dx - 0.5, dy - 0.5, 5.0);
                    Vector3 far(dx - 0.5, dy - 0.5, -5.0);
                    SpaceSegment segment(rotation.multPoint(near.scale(1.3)).add(VECTOR_UP.scale(0.5)),
                                         rotation.multPoint(far.scale(1.3)).add(VECTOR_UP.scale(0.5)));

                    RayCastingResult result = vegetation->renderInstance(segment, instance, false, true);
                    texture->setPixel(startx + x, starty + y,
                                      result.hit ? result.hit_color.normalized() : COLOR_TRANSPARENT);
                }
            }
        }
    }

    texture_changed = true;
}

void OpenGLVegetationImpostor::setVertex(int i, float x, float y, float z) {
    vertices[i * 3] = x;
    vertices[i * 3 + 1] = y;
    vertices[i * 3 + 2] = z;
}
