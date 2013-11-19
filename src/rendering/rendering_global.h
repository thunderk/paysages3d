#ifndef RENDERING_GLOBAL_H
#define RENDERING_GLOBAL_H

#include <QtCore/qglobal.h>
#if defined(RENDERING_LIBRARY)
#  define RENDERINGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define RENDERINGSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "definition_global.h"

class Renderer; // TEMP
class RenderingScenery; // TEMP

#endif // RENDERING_GLOBAL_H
