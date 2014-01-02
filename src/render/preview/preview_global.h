#ifndef PREVIEW_GLOBAL_H
#define PREVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PREVIEW_LIBRARY)
#  define PREVIEWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PREVIEWSHARED_EXPORT Q_DECL_IMPORT
#endif

#include "software_global.h"

namespace paysages {
namespace preview {
    class PreviewOsd;
    class PreviewOsdItem;
    class BasePreview;
    class Base2dPreviewRenderer;
    class AtmosphereColorPreviewRenderer;
    class CloudsCoveragePreviewRenderer;
    class CloudsAspectPreviewRenderer;
    class WaterCoveragePreviewRenderer;
    class WaterAspectPreviewRenderer;
    class TerrainShapePreviewRenderer;
    class TexturesMixPreviewRenderer;
}
}
using namespace paysages::preview;

#endif // PREVIEW_GLOBAL_H
