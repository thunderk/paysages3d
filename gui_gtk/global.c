#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "lib_paysages/shared/functions.h"
#include "lib_paysages/shared/constants.h"
#include "common.h"

GtkBuilder* gui_definition;
static GtkWindow* _main_window;

static void _cbQuit(GtkWidget* widget, gpointer data)
{
    gtk_main_quit();
    exit(0);
}

static void _cbLoad(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Load File",
                          _main_window,
                          GTK_FILE_CHOOSER_ACTION_OPEN,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        paysagesLoad(filename);
        g_free(filename);
    }

    /* Update all GUI */
    guiUpdate();

    gtk_widget_destroy(dialog);
}

static void _cbSaveAs(GtkWidget* widget, gpointer data)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
                          _main_window,
                          GTK_FILE_CHOOSER_ACTION_SAVE,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                          NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        paysagesSave(filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

void guiInit()
{
    GError* p_err = NULL;
    int argc = 0;
    char** argv = NULL;

    gtk_init(&argc, &argv);

    gui_definition = gtk_builder_new();
    gtk_builder_add_from_file(gui_definition, "gui_gtk/gui.glade", &p_err);

    _main_window = GTK_WINDOW(GET_WIDGET("main_window"));
    g_signal_connect(_main_window, "delete_event", G_CALLBACK(_cbQuit), NULL);

    /* Menu bar */
    g_signal_connect(GET_WIDGET("menu_quit"), "activate", G_CALLBACK(_cbQuit), NULL);
    g_signal_connect(GET_WIDGET("menu_load"), "activate", G_CALLBACK(_cbLoad), NULL);
    g_signal_connect(GET_WIDGET("menu_saveas"), "activate", G_CALLBACK(_cbSaveAs), NULL);

    /* Dialogs */
    guiNoiseInit();

    /* Tabs */
    guiRenderInit();
    guiTerrainInit();
    guiWaterInit();
    guiCloudsInit();
    guiSkyInit();
}

void guiStart()
{
    guiPreviewStart();

    guiUpdate();

    gtk_widget_show_all(GTK_WIDGET(_main_window));
    gtk_main();
}

void guiUpdate()
{
    guiWaterUpdate();
    guiCloudsUpdate();
    guiSkyUpdate();

    guiPreviewRedrawAll();
}

int main(int argc, char** argv)
{
    paysagesInit();
    guiInit();
    guiStart();
    return 0;
}
