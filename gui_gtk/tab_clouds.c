/* Terrain tab */

#include "common.h"
#include "lib_paysages/shared/functions.h"
#include "lib_paysages/shared/constants.h"
#include "lib_paysages/clouds.h"
#include <math.h>

static SmallPreview* _preview;
static CloudsDefinition _definition;
static int _current_layer;

static GtkTreeView* _list_layers;
static GtkListStore* _list_layers_model;

/***** Internal functions *****/

static void _revertCurrentLayer()
{
    if (_current_layer >= 0)
    {
        cloudsCopyDefinition(cloudsGetDefinition(_current_layer), &_definition);
    }

    /* TODO Revert layer from config */

    guiPreviewRedraw(_preview);
}

static void _applyCurrentLayer()
{
    /* TODO Apply layer config */

    guiUpdate();
}

static void _revertAll()
{
    int i, n;
    CloudsDefinition layer;
    GtkTreeIter row;

    gtk_list_store_clear(_list_layers_model);
    n = cloudsGetLayerCount();
    for (i = 0 ; i < n; i++)
    {
        layer = cloudsGetDefinition(i);
        gtk_list_store_append(_list_layers_model, &row);
        gtk_list_store_set(_list_layers_model, &row, 0, layer.ymin, 1, layer.ymax - layer.ymin, -1);
    }

    if (_current_layer < 0 || _current_layer >= n)
    {
        _current_layer = -1;
    }
    _revertCurrentLayer();
}

/***** Preview callbacks *****/

static Color _cbPreviewPixel(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling)
{
    Color result, layer_color;
    Vector3 start, end;

    if (_current_layer < 0)
    {
        return COLOR_BLACK;
    }
    else
    {
        start.x = end.x = x;
        start.z = end.z = y;
        start.y = _definition.ymin;
        end.y = _definition.ymax;
        result.r = 0.3;
        result.g = 0.5;
        result.b = 0.8;
        result.a = 1.0;
        layer_color = cloudsGetColorCustom(start, end, &_definition, NULL, NULL);
        colorMask(&result, &layer_color);
        return result;
    }
}

/***** Config callbacks *****/

static void _cbLayerSelected(GtkTreeView* tree_view, gpointer user_data)
{
    GtkTreePath* path;
    GtkTreeViewColumn* column;
    int* indices;

    gtk_tree_view_get_cursor(tree_view, &path, &column);
    indices = gtk_tree_path_get_indices(path);
    if (indices)
    {
        _current_layer = 0;
    }
    else
    {
        _current_layer = -1;
    }
    _revertCurrentLayer();
}

/***** Public functions *****/

void guiCloudsInit()
{
    _list_layers = GTK_TREE_VIEW(GET_WIDGET("clouds_layers"));
    _list_layers_model = GTK_LIST_STORE(gtk_tree_view_get_model(_list_layers));

    _definition = cloudsCreateDefinition();

    _preview = guiPreviewNew(GTK_IMAGE(GET_WIDGET("clouds_preview")));
    guiPreviewConfigScaling(_preview, 1.0, 100.0, 1.0);
    guiPreviewConfigScrolling(_preview, -10000.0, 10000.0, -10000.0, 10000.0);
    guiPreviewSetViewport(_preview, 0.0, 0.0, 10.0);
    guiPreviewSetRenderer(_preview, _cbPreviewPixel);

    g_signal_connect(_list_layers, "cursor-changed", G_CALLBACK(_cbLayerSelected), NULL);

    guiCloudsUpdate();
}

void guiCloudsUpdate()
{
    _revertAll();
}
