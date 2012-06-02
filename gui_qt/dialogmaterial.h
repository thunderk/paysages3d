#ifndef _PAYSAGES_QT_DIALOGMATERIAL_H_
#define _PAYSAGES_QT_DIALOGMATERIAL_H_

#include <QDialog>
#include <QListWidget>
#include "basepreview.h"
#include "formmaterial.h"
#include "tools.h"

#include "../lib_paysages/shared/types.h"

class DialogMaterial : public DialogWithPreview
{
    Q_OBJECT
public:
    explicit DialogMaterial(QWidget* parent, SurfaceMaterial* material);
    static bool getMaterial(QWidget* parent, SurfaceMaterial* material);

public slots:
    virtual void accept();
    void revert();

private:
    FormMaterial* _form;
};

#endif
