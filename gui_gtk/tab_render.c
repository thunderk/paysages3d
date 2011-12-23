/* Terrain tab */

#include "common.h"
#include "lib_paysages/shared/functions.h"
#include "lib_paysages/shared/constants.h"
#include "lib_paysages/shared/globals.h"
#include "lib_paysages/shared/system.h"

static GtkImage* _render_final;
static GdkPixbuf* _render_buffer = NULL;
static guchar* _render_buffer_pixels = NULL;
static int _render_buffer_ymax = 0;
static int _render_buffer_rowstride = 0;
static int _rendering = 0;

static void _previewResize(int width, int height)
{
    if (_render_buffer)
    {
        gdk_pixbuf_unref(_render_buffer);
    }
    _render_buffer = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8, width, height);
    _render_buffer_pixels = gdk_pixbuf_get_pixels(_render_buffer);
    _render_buffer_rowstride = gdk_pixbuf_get_rowstride(_render_buffer);
    _render_buffer_ymax = height - 1;
}

static void _previewClear(Color col)
{
    gdk_pixbuf_fill(_render_buffer, (guint32)colorTo32BitARGB(&col));
}

static void _previewDraw(int x, int y, Color col)
{
    guint32* pixels = (guint32*)(_render_buffer_pixels + (_render_buffer_ymax - y) * _render_buffer_rowstride + x * 4);
    *pixels = (guint32)colorTo32BitRGBA(&col);
}

static void _previewUpdate(double progress)
{
    gtk_image_set_from_pixbuf(_render_final, _render_buffer);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GET_WIDGET("render_progress")), progress);
}

static void* _threadRender(void* data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GET_WIDGET("render_mode_raytracing"))))
    {
        autoRenderSceneRayTracing();
    }
    else
    {
        autoRenderSceneTwoPass(0);
    }
    _rendering = 0;
    return NULL;
}

static void _cbStartRender(GtkWidget* widget, gpointer data)
{
    Thread* thread;

    /* Prepare render */
    renderSetSize(gtk_spin_button_get_value(GTK_SPIN_BUTTON(GET_WIDGET("render_width"))), gtk_spin_button_get_value(GTK_SPIN_BUTTON(GET_WIDGET("render_height"))));
    autoSetRenderQuality((int)gtk_range_get_value(GTK_RANGE(GET_WIDGET("render_quality"))));
    gtk_widget_set_size_request(GET_WIDGET("render_preview"), render_width, render_height);
    gtk_image_clear(GTK_IMAGE(GET_WIDGET("render_preview")));
    renderSetPreviewCallbacks(_previewResize, _previewClear, _previewDraw, _previewUpdate);

    /* Open render dialog */
    gtk_window_set_deletable(GTK_WINDOW(GET_WIDGET("dialog_render")), 0);
    gtk_widget_show(GET_WIDGET("dialog_render"));
    gtk_widget_set_sensitive(GET_WIDGET("render_stop"), 1);
    gtk_widget_set_sensitive(GET_WIDGET("render_close"), 0);

    /* Do the render */
    _rendering = 1;
    thread = threadCreate(_threadRender, NULL);
    while (_rendering)
    {
        timeSleepMs(100);
        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
    }
    threadJoin(thread);

    /* Clean up */
    renderSetPreviewCallbacks(NULL, NULL, NULL, NULL);
    gtk_widget_set_sensitive(GET_WIDGET("render_stop"), 0);
    gtk_widget_set_sensitive(GET_WIDGET("render_close"), 1);
}

static void _cbStopRender(GtkWidget* widget, gpointer data)
{
    if (_rendering)
    {
        renderInterrupt();
    }
}

static void _cbCloseRender(GtkWidget* widget, gpointer data)
{
    gtk_widget_hide(GET_WIDGET("dialog_render"));
}

static void _cbShowRender(GtkWidget* widget, gpointer data)
{
    gtk_widget_show(GET_WIDGET("dialog_render"));
}

static void _cbSaveRender(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog;
    char *filename;

    dialog = gtk_file_chooser_dialog_new("Save last render",
                                         GTK_WINDOW(GET_WIDGET("main_window")),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                         NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "render.png");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        renderSaveToFile(filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static int _cbWindowClosed(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    gtk_widget_hide(widget);
    if (_rendering)
    {
        renderInterrupt();
    }
    return 1;
}

void guiRenderInit()
{
    _render_final = GTK_IMAGE(GET_WIDGET("render_preview"));
    _previewResize(800, 600);
    _previewClear(COLOR_BLACK);
    _previewUpdate(0.0);

    g_signal_connect(GET_WIDGET("render_show"), "clicked", G_CALLBACK(_cbShowRender), NULL);
    g_signal_connect(GET_WIDGET("render_save"), "clicked", G_CALLBACK(_cbSaveRender), NULL);
    g_signal_connect(GET_WIDGET("render_start"), "clicked", G_CALLBACK(_cbStartRender), NULL);
    g_signal_connect(GET_WIDGET("render_stop"), "clicked", G_CALLBACK(_cbStopRender), NULL);
    g_signal_connect(GET_WIDGET("render_close"), "clicked", G_CALLBACK(_cbCloseRender), NULL);
    g_signal_connect(GET_WIDGET("dialog_render"), "delete-event", G_CALLBACK(_cbWindowClosed), NULL);

    gtk_range_set_range(GTK_RANGE(GET_WIDGET("render_quality")), 1, 10);
    gtk_range_set_value(GTK_RANGE(GET_WIDGET("render_quality")), 5);

    gtk_spin_button_set_range(GTK_SPIN_BUTTON(GET_WIDGET("render_width")), 100, 4000);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(GET_WIDGET("render_width")), 1200);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(GET_WIDGET("render_width")), 10, 100);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(GET_WIDGET("render_height")), 100, 4000);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(GET_WIDGET("render_height")), 900);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(GET_WIDGET("render_height")), 10, 100);
}

