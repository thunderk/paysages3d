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
    class BaseExplorerChunk;
}
}
using namespace paysages::opengl;

#endif // OPENGL_GLOBAL_H
