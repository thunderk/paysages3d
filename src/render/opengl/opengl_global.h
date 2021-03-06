#pragma once

#include "software_global.h"

#if defined(OPENGL_LIBRARY)
#define OPENGLSHARED_EXPORT DECL_EXPORT
#else
#define OPENGLSHARED_EXPORT DECL_IMPORT
#endif

namespace paysages {
namespace opengl {
class OpenGLRenderer;
class OpenGLPart;
class OpenGLShaderProgram;
class OpenGLSharedState;
class OpenGLVariable;
class OpenGLVertexArray;
class OpenGLSkybox;
class OpenGLWater;
class OpenGLTerrain;
class OpenGLVegetation;
class OpenGLVegetationLayer;
class OpenGLVegetationInstance;
class OpenGLVegetationImpostor;
class OpenGLTerrainChunk;
}
}
using namespace paysages::opengl;

#define OpenGLFunctions QOpenGLFunctions_3_3_Core
#define OPENGL_GLSL_VERSION "330 core"
#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3
class OpenGLFunctions;
