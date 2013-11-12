#ifndef MAINTERRAINFORM_H
#define MAINTERRAINFORM_H

#include "editing_global.h"

#include <QWidget>

#include "terrain/public.h"
class FreeFormHelper;

namespace Ui {
class MainTerrainForm;
}

class MainTerrainForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainTerrainForm(QWidget *parent = 0);
    ~MainTerrainForm();

    inline TerrainDefinition* getTerrainDefinition() {return _terrain;}

public slots:
    void refreshFromLocalData();
    void refreshFromFellowData();
    void updateLocalDataFromScenery();
    void commitLocalDataToScenery();
    void alterRenderer(Renderer* renderer);

    void buttonBaseNoisePressed();
    void buttonPaintingPressed();
    void buttonTexturesPressed();

private:
    Ui::MainTerrainForm *ui;
    FreeFormHelper* _form_helper;

    TerrainDefinition* _terrain;

    Base2dPreviewRenderer* _renderer_shape;
};

#endif // MAINTERRAINFORM_H
