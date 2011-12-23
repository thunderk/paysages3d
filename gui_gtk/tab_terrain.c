/* Terrain tab */

#include "common.h"
#include "lib_paysages/shared/functions.h"

static SmallPreview* _preview;

static Color _cbPreviewRenderPixel(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Color result;

    result.r = result.g = result.b = terrainGetHeightNormalized(x, y);
    result.a = 1.0;

    return result;
}

static void _cbEditNoiseDone(NoiseGenerator* generator)
{
    terrainSetNoiseGenerator(generator);
    /* TODO Redraw only affected by terrain */
    guiPreviewRedrawAll();
}

static void _cbEditNoise(GtkWidget* widget, gpointer data)
{
    guiNoiseEdit(terrainGetNoiseGenerator(), _cbEditNoiseDone);
}

void guiTerrainInit()
{
    g_signal_connect(GET_WIDGET("terrain_noise_edit"), "clicked", G_CALLBACK(_cbEditNoise), NULL);

    _preview = guiPreviewNew(GTK_IMAGE(GET_WIDGET("terrain_preview")));
    guiPreviewConfigScaling(_preview, 0.01, 1.0, 0.05);
    guiPreviewConfigScrolling(_preview, -1000.0, 1000.0, -1000.0, 1000.0);
    guiPreviewSetViewport(_preview, 0.0, 0.0, 0.2);
    guiPreviewSetRenderer(_preview, _cbPreviewRenderPixel);
}

