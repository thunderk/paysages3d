/* Noise editor dialog */

#include "common.h"
#include "../shared/functions.h"
#include "../shared/constants.h"

static GtkWidget* _dialog;
static SmallPreview* _preview;
static GtkTreeView* _level_list;
static GtkListStore* _level_list_model;
static NoiseGenerator* _generator;
static GuiNoiseChangedCallback _callback;
static int _current_mode;

static Color _cbPreview1DRenderPixel(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    if (y > noiseGet1DTotal(_generator, x))
    {
        return COLOR_WHITE;
    }
    else
    {
        return COLOR_BLACK;
    }
}

static Color _cbPreview2DRenderPixel(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Color col;
    double max_value;
    
    /* TODO Cache this value */
    max_value = noiseGetMaxValue(_generator);

    col.r = col.g = col.b = (noiseGet2DTotal(_generator, x, y) / max_value) * 0.5 + 0.5;
    col.a = 1.0;
    
    return col;
}

static void _setPreviewMode(int mode)
{
    GtkButton* button;
    double max_value;
    
    max_value = noiseGetMaxValue(_generator);
    
    button = GTK_BUTTON(GET_WIDGET("noise_editor_preview_mode"));
    if (mode == 1)
    {
        _current_mode = 1;
        guiPreviewSetRenderer(_preview, _cbPreview1DRenderPixel);
        gtk_button_set_label(button, "1D");
        
        guiPreviewConfigScrolling(_preview, -max_value * 100.0, max_value * 100.0, -max_value, max_value);
    }
    else if (mode == 2)
    {
        _current_mode = 2;
        guiPreviewSetRenderer(_preview, _cbPreview2DRenderPixel);
        gtk_button_set_label(button, "2D");
        
        guiPreviewConfigScrolling(_preview, -max_value * 100.0, max_value * 100.0, -max_value * 100.0, max_value * 100.0);
    }
    
    guiPreviewConfigScaling(_preview, max_value * 0.001, max_value * 0.1, max_value * 0.001);
    guiPreviewSetViewport(_preview, 0.0, 0.0, max_value * 0.01);
}

static void _redrawPreview()
{
    guiPreviewRedraw(_preview);
}

static void _resetPreview()
{
    _setPreviewMode(_current_mode);
    
    _redrawPreview();
}

static void _applyLayerParams()
{
    GtkTreePath* path;
    GtkTreeViewColumn* column;
    GtkTreeIter row;
    int* indices;
    double height, scale;

    gtk_tree_view_get_cursor(_level_list, &path, &column);
    indices = gtk_tree_path_get_indices(path);
    if (indices)
    {
        height = (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_height")));
        scale = (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_scale")));

        noiseSetLevelSimple(_generator, indices[0], scale, height);

        gtk_tree_model_get_iter(GTK_TREE_MODEL(_level_list_model), &row, path);
        gtk_list_store_set(_level_list_model, &row, 0, height, 1, scale, -1);
    }
    _redrawPreview();
}

static void _cbGenerateClicked(GtkButton* button, gpointer user_data)
{
    noiseGenerateBaseNoise(_generator, gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_base_size"))));
    _redrawPreview();
}

static void _cbPreviewModeClicked(GtkButton* button, gpointer user_data)
{
    if (_current_mode == 1)
    {
        _setPreviewMode(2);
    }
    else
    {
        _setPreviewMode(1);
    }
    _redrawPreview();
}

static void _cbPreviewResetClicked(GtkButton* button, gpointer user_data)
{
    _resetPreview();
}

static void _cbApplyClicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(_dialog);
    _callback(_generator);
}

static void _cbCancelClicked(GtkButton* button, gpointer user_data)
{
    gtk_widget_hide(_dialog);
}

static void _cbLevelParamChanged(GtkWidget* widget, gpointer user_data)
{
    _applyLayerParams();
}

static void _cbRowSelected(GtkTreeView* tree_view, gpointer user_data)
{
    GtkTreePath* path;
    GtkTreeViewColumn* column;
    int* indices;
    NoiseLevel params;

    gtk_tree_view_get_cursor(tree_view, &path, &column);
    indices = gtk_tree_path_get_indices(path);
    if (indices)
    {
        if (noiseGetLevel(_generator, indices[0], &params))
        {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_height")), params.height);
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_scale")), params.scaling);
        }
    }
}

void guiNoiseInit()
{
    _dialog = GET_WIDGET("dialog_noise");
    _level_list = GTK_TREE_VIEW(GET_WIDGET("noise_editor_levels"));
    _level_list_model = GTK_LIST_STORE(gtk_tree_view_get_model(_level_list));
    _generator = noiseCreateGenerator();

    gtk_spin_button_set_range(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_base_size")), 1, 4000000);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_base_size")), 100, 100000);

    gtk_spin_button_set_range(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_scale")), 0.0001, 100.0);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_scale")), 0.0001, 0.1);
    gtk_spin_button_set_range(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_height")), 0.0, 100.0);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_height")), 0.0001, 0.1);

    _preview = guiPreviewNew(GTK_IMAGE(GET_WIDGET("noise_editor_preview")));
    _resetPreview();

    g_signal_connect(_dialog, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_generate"), "clicked", G_CALLBACK(_cbGenerateClicked), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_preview_reset"), "clicked", G_CALLBACK(_cbPreviewResetClicked), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_preview_mode"), "clicked", G_CALLBACK(_cbPreviewModeClicked), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_apply"), "clicked", G_CALLBACK(_cbApplyClicked), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_cancel"), "clicked", G_CALLBACK(_cbCancelClicked), NULL);
    g_signal_connect(_level_list, "cursor-changed", G_CALLBACK(_cbRowSelected), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_height"), "value-changed", G_CALLBACK(_cbLevelParamChanged), NULL);
    g_signal_connect(GET_WIDGET("noise_editor_scale"), "value-changed", G_CALLBACK(_cbLevelParamChanged), NULL);
}

void guiNoiseEdit(NoiseGenerator* generator, GuiNoiseChangedCallback callback)
{
    int i, n;
    NoiseLevel level;
    GtkTreeIter row;

    _callback = callback;
    noiseCopy(generator, _generator);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(GET_WIDGET("noise_editor_base_size")), noiseGetBaseSize(_generator));

    gtk_list_store_clear(_level_list_model);
    n = noiseGetLevelCount(_generator);
    for (i = 0 ; i < n; i++)
    {
        noiseGetLevel(_generator, i, &level);
        gtk_list_store_append(_level_list_model, &row);
        gtk_list_store_set(_level_list_model, &row, 0, level.height, 1, level.scaling, -1);
    }

    _setPreviewMode(1);
    _resetPreview();

    gtk_widget_show(_dialog);
}

