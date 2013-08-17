#ifndef MAINTEXTURESFORM_H
#define MAINTEXTURESFORM_H

#include <QWidget>
#include "rendering/textures/public.h"

namespace Ui {
class MainTexturesForm;
}

class FreeFormHelper;
class FreeLayerHelper;

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

private:
    Ui::MainTexturesForm *ui;
    TexturesDefinition* textures;
    FreeFormHelper* form_helper;
    FreeLayerHelper* layer_helper;
};

#endif // MAINTEXTURESFORM_H
