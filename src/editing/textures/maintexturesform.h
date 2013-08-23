#ifndef MAINTEXTURESFORM_H
#define MAINTEXTURESFORM_H

#include <QWidget>
#include "rendering/textures/public.h"

namespace Ui {
class MainTexturesForm;
}

class FreeFormHelper;
class FreeLayerHelper;
class PreviewLayerCoverage;
class PreviewLayerLook;
class PreviewCumul;

class MainTexturesForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainTexturesForm(QWidget *parent = 0);
    ~MainTexturesForm();

public slots:
    void updateLocalDataFromScenery();
    void commitLocalDataToScenery();
    void refreshFromLocalData();
    void refreshFromFellowData();
    void alterRenderer(Renderer* renderer);

    void updateLayers();
    void selectLayer(int layer);
    void selectPreset(int preset);

private:
    Ui::MainTexturesForm *ui;
    TexturesDefinition* textures;
    FreeFormHelper* form_helper;
    FreeLayerHelper* layer_helper;
    PreviewLayerCoverage* preview_layer_coverage;
    PreviewLayerLook* preview_layer_look;
    PreviewCumul* preview_cumul;
};

#endif // MAINTEXTURESFORM_H
