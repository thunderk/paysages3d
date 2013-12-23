#ifndef OPENGL_GLOBAL_H
#define OPENGL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(OPENGL_LIBRARY)
#  define OPENGLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define OPENGLSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "software_global.h"

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

//#define OpenGLFunctions QOpenGLFunctions_3_2_Core
#define OpenGLFunctions QOpenGLFunctions_3_0
#define OPENGL_FUNCTIONS_INCLUDE <OpenGLFunctions>
class OpenGLFunctions;

#endif // OPENGL_GLOBAL_H
