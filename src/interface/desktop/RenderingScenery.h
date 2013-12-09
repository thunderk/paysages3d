#ifndef RENDERINGSCENERY_H
#define RENDERINGSCENERY_H

#include "desktop_global.h"

#include "Scenery.h"

typedef void (*SceneryCustomDataCallback)(PackStream* stream, void* data);

/**
 * @brief Global scenery management
 *
 * This class contains the whole scenery definition.
 */
class RenderingScenery: public Scenery
{
public:
    RenderingScenery();

    static RenderingScenery* getCurrent();

    void setCustomSaveCallbacks(SceneryCustomDataCallback callback_save, SceneryCustomDataCallback callback_load, void* data);

    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

private:
    SceneryCustomDataCallback _custom_save;
    SceneryCustomDataCallback _custom_load;
    void* _custom_data;
};

#endif // RENDERINGSCENERY_H
