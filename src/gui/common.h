#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "../shared/types.h"

#include <stdio.h>

typedef void (*GuiNoiseChangedCallback)(NoiseGenerator* generator);

typedef struct SmallPreview SmallPreview;
typedef Color (*SmallPreviewCallback)(SmallPreview* preview, double x, double y, double xoffset, double yoffset, double scaling);

extern GtkBuilder* gui_definition;

static inline GtkWidget* _get_widget(const char* name, const char* file, int line)
{
    GtkWidget* result;
    result = (GtkWidget*)gtk_builder_get_object(gui_definition, (name));
    if (result == NULL)
    {
        printf("Widget not found (%s:%d) : %s\n", file, line, name);
    }
    return result;
}
#define GET_WIDGET(_name_) (_get_widget(_name_, __FILE__, __LINE__))

void guiTerrainInit();

void guiWaterInit();
void guiWaterUpdate();

void guiSkyInit();
void guiSkyUpdate();

void guiCloudsInit();
void guiCloudsUpdate();

void guiRenderInit();

void guiNoiseInit();
void guiNoiseEdit(NoiseGenerator* generator, GuiNoiseChangedCallback callback);

void guiPreviewStart();
void guiPreviewRedraw(SmallPreview* preview);
void guiPreviewRedrawAll();
SmallPreview* guiPreviewNew(GtkImage* image);
void guiPreviewSetTerrainHeight(SmallPreview* preview);
void guiPreviewSetTerrainColor(SmallPreview* preview);
void guiPreviewSetNoise1D(SmallPreview* preview, NoiseGenerator* generator);
void guiPreviewConfigScrolling(SmallPreview* preview, double xmin, double xmax, double ymin, double ymax);
void guiPreviewConfigScaling(SmallPreview* preview, double min, double max, double step);
void guiPreviewSetRenderer(SmallPreview* preview, SmallPreviewCallback renderer);
void guiPreviewSetViewport(SmallPreview* preview, double xoffset, double yoffset, double scaling);
