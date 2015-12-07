#ifndef OPENGL_GLOBAL_H
#define OPENGL_GLOBAL_H

#include "software_global.h"

#if defined(OPENGL_LIBRARY)
#define OPENGLSHARED_EXPORT Q_DECL_EXPORT
#else
#define OPENGLSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace paysages {
namespace opengl {
class WidgetExplorer;
class OpenGLRenderer;
class OpenGLShaderProgram;
class OpenGLSharedState;
class OpenGLVariable;
class OpenGLVertexArray;
class OpenGLSkybox;
class OpenGLWater;
class OpenGLTerrain;
class OpenGLTerrainChunk;
template <typename Vertex> class VertexArray;
}
}
using namespace paysages::opengl;

#define OpenGLFunctions QOpenGLFunctions_3_3_Core
#define OPENGL_GLSL_VERSION "330 core"
#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3
class OpenGLFunctions;

#endif // OPENGL_GLOBAL_H
