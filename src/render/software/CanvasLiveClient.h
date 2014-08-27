#ifndef CANVASLIVECLIENT_H
#define CANVASLIVECLIENT_H

#include "software_global.h"

namespace paysages {
namespace software {

/**
 * @brief Abstract class to receive live modifications from canvas preview.
 */
class SOFTWARESHARED_EXPORT CanvasLiveClient
{
public:
    CanvasLiveClient();

    virtual void canvasResized(int width, int height);
    virtual void canvasCleared(const Color &col);
    virtual void canvasPainted(int x, int y, const Color &col);
};

}
}

#endif // CANVASLIVECLIENT_H
