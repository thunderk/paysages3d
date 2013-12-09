#ifndef DIALOGMATERIALEDITOR_H
#define DIALOGMATERIALEDITOR_H

#include "desktop_global.h"

#include <QDialog>

#include "previewmaterial.h"
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
    void alterRenderer(SoftwareRenderer* renderer);

private:
    Ui::DialogMaterialEditor *ui;

    SurfaceMaterial* original;
    SurfaceMaterial edited;

    MaterialPreviewRenderer preview_lighted;

    FreeFormHelper* form_helper;
};

#endif // DIALOGMATERIALEDITOR_H
