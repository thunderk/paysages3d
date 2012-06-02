#include "dialogmaterial.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

/**************** Dialog form ****************/
DialogMaterial::DialogMaterial(QWidget *parent, SurfaceMaterial* material) : DialogWithPreview(parent)
{
    QPushButton* button;
    QWidget* buttons;
    
    setLayout(new QVBoxLayout());
    
    _form = new FormMaterial(this, material);
    _form->hideButtons();
    layout()->addWidget(_form);
    
    buttons = new QWidget(this);
    buttons->setLayout(new QHBoxLayout());
    layout()->addWidget(buttons);
    layout()->setAlignment(buttons, Qt::AlignBottom);

    button = new QPushButton(tr("Validate"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(accept()));

    button = new QPushButton(tr("Reset"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(revert()));

    button = new QPushButton(tr("Cancel"), buttons);
    buttons->layout()->addWidget(button);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Paysages 3D - Material editor"));
}

bool DialogMaterial::getMaterial(QWidget* parent, SurfaceMaterial* material)
{
    int result;

    DialogMaterial* dialog = new DialogMaterial(parent, material);
    result = dialog->exec();
    
    if (result)
    {
        dialog->_form->getMaterial(material);
    }

    delete dialog;

    return (result != 0) ? true : false;
}

void DialogMaterial::accept()
{
    _form->applyConfig();
    QDialog::accept();
}

void DialogMaterial::revert()
{
    _form->revertConfig();
}
