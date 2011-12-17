/* Terrain tab */

#include "common.h"
#include "../shared/functions.h"
#include "../shared/constants.h"
#include "../sky.h"
#include <math.h>

static SmallPreview* _preview_horizon;
static SkyDefinition _definition;

static Color _cbPreviewHorizon(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Vector3 eye = {0.0, 0.0, 0.0};
    Vector3 look;

    look.x = x;
    look.y = -y;
    look.z = 1.0;

    return skyGetColorCustom(eye, look, &_definition, NULL, NULL);
}

static inline void _updatePreview()
{
    skyValidateDefinition(&_definition);
    guiPreviewRedraw(_preview_horizon);
}

static void _redrawColorGradation(GtkImage* image, ColorGradation* gradation)
{
    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8, 200, 30);
    void* pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    int x, y;
    guint32* pixel;
    Color col;

    for (x = 0; x < 200; x++)
    {
        for (y = 0; y < 30; y++)
        {
            pixel = (guint32*)(pixels + y * rowstride + x * 4);
            col = colorGradationGet(gradation, (double)x / 200.0);
            *pixel = (guint32)colorTo32BitRGBA(&col);
        }
    }

    gtk_image_set_from_pixbuf(image, pixbuf);
    gdk_pixbuf_unref(pixbuf);
}

static void _cbDaytimeChanged(GtkRange* range, gpointer data)
{
    _definition.daytime = gtk_range_get_value(range);
    _updatePreview();
}

static void _cbSunSizeChanged(GtkRange* range, gpointer data)
{
    _definition.sun_radius = gtk_range_get_value(range);
    _updatePreview();
}

static void _cbHazeHeightChanged(GtkRange* range, gpointer data)
{
    _definition.haze_height = gtk_range_get_value(range);
    _updatePreview();
}

static void _cbHazeSmoothingChanged(GtkRange* range, gpointer data)
{
    _definition.haze_smoothing = gtk_range_get_value(range);
    _updatePreview();
}

static void _cbRevertConfig(GtkWidget* widget, gpointer data)
{
    skyCopyDefinition(skyGetDefinition(), &_definition);

    gtk_range_set_value(GTK_RANGE(GET_WIDGET("sky_daytime")), _definition.daytime);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("sky_sun_size")), _definition.sun_radius);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("sky_haze_height")), _definition.haze_height);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("sky_haze_smoothing")), _definition.haze_smoothing);

    _redrawColorGradation(GTK_IMAGE(GET_WIDGET("sky_colorgradient_sun")), &_definition.sun_color);
    _redrawColorGradation(GTK_IMAGE(GET_WIDGET("sky_colorgradient_zenith")),& _definition.zenith_color);
    _redrawColorGradation(GTK_IMAGE(GET_WIDGET("sky_colorgradient_haze")), &_definition.haze_color);

    _updatePreview();
}

static void _cbApplyConfig(GtkWidget* widget, gpointer data)
{
    skySetDefinition(_definition);
    guiUpdate();
}

void guiSkyInit()
{
    _definition = skyCreateDefinition();

    /* Buttons */
    g_signal_connect(GET_WIDGET("sky_apply"), "clicked", G_CALLBACK(_cbApplyConfig), NULL);
    g_signal_connect(GET_WIDGET("sky_revert"), "clicked", G_CALLBACK(_cbRevertConfig), NULL);

    /* Configs */
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("sky_daytime")), 0.0, 1.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("sky_sun_size")), 0.0, 1.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("sky_haze_height")), 0.0, 1.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("sky_haze_smoothing")), 0.0, 1.0);

    /* Config signals */
    g_signal_connect(GET_WIDGET("sky_daytime"), "value-changed", G_CALLBACK(_cbDaytimeChanged), NULL);
    g_signal_connect(GET_WIDGET("sky_sun_size"), "value-changed", G_CALLBACK(_cbSunSizeChanged), NULL);
    g_signal_connect(GET_WIDGET("sky_haze_height"), "value-changed", G_CALLBACK(_cbHazeHeightChanged), NULL);
    g_signal_connect(GET_WIDGET("sky_haze_smoothing"), "value-changed", G_CALLBACK(_cbHazeSmoothingChanged), NULL);

    /* Previews */
    _preview_horizon = guiPreviewNew(GTK_IMAGE(GET_WIDGET("sky_preview_horizon")));
    guiPreviewConfigScaling(_preview_horizon, 0.01, 0.01, 0.0);
    guiPreviewConfigScrolling(_preview_horizon, 0.0, 0.0, 0.0, 0.0);
    guiPreviewSetViewport(_preview_horizon, 0.0, 0.0, 0.01);
    guiPreviewSetRenderer(_preview_horizon, _cbPreviewHorizon);

    guiSkyUpdate();
}

void guiSkyUpdate()
{
    _cbRevertConfig(NULL, NULL);
}
