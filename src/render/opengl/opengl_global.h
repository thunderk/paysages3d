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
class OpenGLSkybox;
class OpenGLWater;
class OpenGLTerrain;
class ExplorerChunkTerrain;
template <typename Vertex> class VertexArray;
}
}
using namespace paysages::opengl;

#define OpenGLFunctions QOpenGLFunctions_3_0
//#define OpenGLFunctions QOpenGLFunctions_3_3_Core
class OpenGLFunctions;

#endif // OPENGL_GLOBAL_H
