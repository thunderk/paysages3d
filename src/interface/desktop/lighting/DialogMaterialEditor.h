#ifndef DIALOGMATERIALEDITOR_H
#define DIALOGMATERIALEDITOR_H

#include <QDialog>

#include "previewmaterial.h"

#include "tools/lighting.h"
#include "renderer.h"
#include "SurfaceMaterial.h"

namespace Ui {
class DialogMaterialEditor;
}

class FreeFormHelper;

class DialogMaterialEditor : public QDialog
{
    Q_OBJECT

public:
    DialogMaterialEditor(QWidget *parent, SurfaceMaterial* material);
    ~DialogMaterialEditor();

    static bool getMaterial(QWidget* parent, SurfaceMaterial* material);

public slots:
    void refreshFromLocalData();
    void refreshFromFellowData();
    void updateLocalDataFromScenery();
    void commitLocalDataToScenery();
    void alterRenderer(Renderer* renderer);

private:
    Ui::DialogMaterialEditor *ui;

    SurfaceMaterial* original;
    SurfaceMaterial edited;

    MaterialPreviewRenderer preview_lighted;

    FreeFormHelper* form_helper;
};

#endif // DIALOGMATERIALEDITOR_H
