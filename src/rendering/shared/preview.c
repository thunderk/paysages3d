#include "preview.h"

#include <stdlib.h>
#include "../renderer.h"
#include "../camera.h"

struct PreviewRenderer
{
    Renderer* renderer;
};

static PreviewRenderer* _createRenderer()
{
    PreviewRenderer* renderer;

    renderer = (PreviewRenderer*)malloc(sizeof(PreviewRenderer));
    renderer->renderer = rendererCreate();

    return renderer;
}

static void _deleteRenderer(PreviewRenderer* renderer)
{
    rendererDelete(renderer->renderer);
    free(renderer);
}

static void _setCamera(PreviewRenderer* renderer, Vector3 location, Vector3 target)
{
    cameraSetLocation(&renderer->renderer->render_camera, location.x, location.y, location.z);
    cameraSetTarget(&renderer->renderer->render_camera, target.x, target.y, target.z);
}

PreviewClass previewCreateClass(FuncPreviewCustomizeRenderer customizeRenderer, FuncPreviewBindDefinition bindDefinition, FuncPreviewGetPixelColor getPixelColor)
{
    PreviewClass result;

    result.createRenderer = _createRenderer;
    result.deleteRenderer = _deleteRenderer;

    result.setCamera = _setCamera;

    result.customizeRenderer = customizeRenderer;
    result.bindDefinition = bindDefinition;
    result.getPixelColor = getPixelColor;

    return result;
}
