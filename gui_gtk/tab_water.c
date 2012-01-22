/* Terrain tab */

#include "common.h"
#include "lib_paysages/shared/functions.h"
#include "lib_paysages/shared/constants.h"
#include "lib_paysages/water.h"
#include "lib_paysages/terrain.h"
#include <math.h>

static SmallPreview* _preview_coverage;
static SmallPreview* _preview_render;
static WaterDefinition _definition;

static RayCastingResult _rayCastFromWater(Vector3 start, Vector3 direction)
{
    RayCastingResult result;
    double x, y;

    result.hit = 1;
    if (direction.z < 0.0001)
    {
        result.hit_color = COLOR_WHITE;
    }
    else
    {
        x = start.x + direction.x * (0.0 - start.z) / direction.z;
        y = start.y + direction.y * (0.0 - start.z) / direction.z;

        if (((int)ceil(x * 0.2) % 2 == 0) ^ ((int)ceil(y * 0.2 - 0.5) % 2 == 0))
        {
            result.hit_color = COLOR_WHITE;
        }
        else
        {
            result.hit_color = COLOR_GREY;
        }
    }
    /* TODO hit_location */

    return result;
}

static Color _cbPreviewCoverage(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Color result;
    double height;

    height = terrainGetHeight(x, y);
    if (height <= _definition.height)
    {
        return _definition.main_color;
    }
    else
    {
        result.r = result.g = result.b = terrainGetHeightNormalized(x, y);
        result.a = 1.0;

        return result;
    }
}

static Color _cbPreviewRender(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Vector3 eye, look, location;
    WaterDefinition definition;
    WaterEnvironment environment;
    WaterQuality quality;

    eye.x = 0.0;
    eye.y = scaling;
    eye.z = -10.0 * scaling;
    look.x = x * 0.01 / scaling;
    look.y = -y * 0.01 / scaling - 0.3;
    look.z = 1.0;
    look = v3Normalize(look);

    if (look.y > -0.0001)
    {
        return _rayCastFromWater(eye, look).hit_color;
    }

    location.x = eye.x - look.x * eye.y / look.y;
    location.y = 0.0;
    location.z = eye.z - look.z * eye.y / look.y;

    if (location.z > 0.0)
    {
        return _rayCastFromWater(eye, look).hit_color;
    }

    definition = _definition;
    definition.height = 0.0;
    environment.reflection_function = _rayCastFromWater;
    environment.refraction_function = _rayCastFromWater;
    environment.toggle_fog = 0;
    quality.force_detail = 0.0001;

    return waterGetColorCustom(location, look, &definition, &quality, &environment).final;
}

static void _cbEditNoiseDone(NoiseGenerator* generator)
{
    noiseCopy(generator, _definition.waves_noise);
    guiPreviewRedraw(_preview_render);
}

static void _cbEditNoise(GtkWidget* widget, gpointer data)
{
    guiNoiseEdit(_definition.waves_noise, _cbEditNoiseDone);
}

static void _cbHeightChanged(GtkRange* range, gpointer data)
{
    _definition.height = gtk_range_get_value(range);
    guiPreviewRedraw(_preview_coverage);
}

static void _cbTransparencyChanged(GtkRange* range, gpointer data)
{
    _definition.transparency = gtk_range_get_value(range);
    guiPreviewRedraw(_preview_render);
}

static void _cbReflectionChanged(GtkRange* range, gpointer data)
{
    _definition.reflection = gtk_range_get_value(range);
    guiPreviewRedraw(_preview_render);
}

static void _cbTransparencyDepthChanged(GtkRange* range, gpointer data)
{
    _definition.transparency_depth = gtk_range_get_value(range);
    guiPreviewRedraw(_preview_render);
}

static void _cbColorChanged(GtkColorButton* colorbutton, gpointer data)
{
    GdkColor col;

    gtk_color_button_get_color(colorbutton, &col);
    _definition.main_color.r = (double)col.red / 65535.0;
    _definition.main_color.g = (double)col.green / 65535.0;
    _definition.main_color.b = (double)col.blue / 65535.0;
    _definition.main_color.a = 1.0;

    guiPreviewRedraw(_preview_render);
    guiPreviewRedraw(_preview_coverage);
}

static void _cbColorDepthChanged(GtkColorButton* colorbutton, gpointer data)
{
    GdkColor col;

    gtk_color_button_get_color(colorbutton, &col);
    _definition.depth_color.r = (double)col.red / 65535.0;
    _definition.depth_color.g = (double)col.green / 65535.0;
    _definition.depth_color.b = (double)col.blue / 65535.0;
    _definition.depth_color.a = 1.0;

    guiPreviewRedraw(_preview_render);
    guiPreviewRedraw(_preview_coverage);
}

static void _cbRevertConfig(GtkWidget* widget, gpointer data)
{
    GdkColor col;

    waterCopyDefinition(waterGetDefinition(), &_definition);

    gtk_range_set_value(GTK_RANGE(GET_WIDGET("water_height")), _definition.height);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("water_transparency")), _definition.transparency);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("water_reflection")), _definition.reflection);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("water_transparency_depth")), _definition.transparency_depth);
    col.red = _definition.main_color.r * 65535.0;
    col.green = _definition.main_color.g * 65535.0;
    col.blue = _definition.main_color.b * 65535.0;
    gtk_color_button_set_color(GTK_COLOR_BUTTON(GET_WIDGET("water_color")), &col);
    col.red = _definition.depth_color.r * 65535.0;
    col.green = _definition.depth_color.g * 65535.0;
    col.blue = _definition.depth_color.b * 65535.0;
    gtk_color_button_set_color(GTK_COLOR_BUTTON(GET_WIDGET("water_color_depth")), &col);

    guiPreviewRedraw(_preview_render);
    guiPreviewRedraw(_preview_coverage);
}

static void _cbApplyConfig(GtkWidget* widget, gpointer data)
{
    waterSetDefinition(_definition);
    guiUpdate();
}

void guiWaterInit()
{
    _definition = waterCreateDefinition();

    /* Buttons */
    g_signal_connect(GET_WIDGET("water_noise_edit"), "clicked", G_CALLBACK(_cbEditNoise), NULL);
    g_signal_connect(GET_WIDGET("water_apply"), "clicked", G_CALLBACK(_cbApplyConfig), NULL);
    g_signal_connect(GET_WIDGET("water_revert"), "clicked", G_CALLBACK(_cbRevertConfig), NULL);

    /* Configs */
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("water_height")), -20.0, 20.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("water_transparency")), 0.0, 1.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("water_reflection")), 0.0, 1.0);
    gtk_range_set_range(GTK_RANGE(GET_WIDGET("water_transparency_depth")), 0.0, 100.0);

    /* Config signals */
    g_signal_connect(GET_WIDGET("water_height"), "value-changed", G_CALLBACK(_cbHeightChanged), NULL);
    g_signal_connect(GET_WIDGET("water_transparency"), "value-changed", G_CALLBACK(_cbTransparencyChanged), NULL);
    g_signal_connect(GET_WIDGET("water_transparency_depth"), "value-changed", G_CALLBACK(_cbTransparencyDepthChanged), NULL);
    g_signal_connect(GET_WIDGET("water_reflection"), "value-changed", G_CALLBACK(_cbReflectionChanged), NULL);
    g_signal_connect(GET_WIDGET("water_color"), "color-set", G_CALLBACK(_cbColorChanged), NULL);
    g_signal_connect(GET_WIDGET("water_color_depth"), "color-set", G_CALLBACK(_cbColorDepthChanged), NULL);

    /* Previews */
    _preview_coverage = guiPreviewNew(GTK_IMAGE(GET_WIDGET("water_preview_coverage")));
    guiPreviewConfigScaling(_preview_coverage, 0.01, 1.0, 0.05);
    guiPreviewConfigScrolling(_preview_coverage, -1000.0, 1000.0, -1000.0, 1000.0);
    guiPreviewSetViewport(_preview_coverage, 0.0, 0.0, 0.2);
    guiPreviewSetRenderer(_preview_coverage, _cbPreviewCoverage);
    _preview_render = guiPreviewNew(GTK_IMAGE(GET_WIDGET("water_preview_render")));
    guiPreviewConfigScaling(_preview_render, 0.1, 1.0, 0.1);
    guiPreviewConfigScrolling(_preview_render, -10.0, 10.0, -10.0, 10.0);
    guiPreviewSetViewport(_preview_render, 0.0, 0.0, 0.5);
    guiPreviewSetRenderer(_preview_render, _cbPreviewRender);

    guiWaterUpdate();
}

void guiWaterUpdate()
{
    _cbRevertConfig(NULL, NULL);
}
