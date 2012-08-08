#include "dialoglayers.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include "baseformlayer.h"

/**************** Dialog form ****************/
DialogLayers::DialogLayers(QWidget *parent, Layers* layers, QString title, FormLayerBuilder form_builder) : DialogWithPreview(parent)
{
    QPushButton* button;
    QWidget* buttons;
    
    _layers = layers;
    
    setLayout(new QVBoxLayout());
    
    _form = form_builder(this, _layers);
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

    setWindowTitle(tr("Paysages 3D - ") + title);
}

DialogLayers::~DialogLayers()
{
}

bool DialogLayers::editLayers(QWidget* parent, Layers* layers, QString title, FormLayerBuilder form_builder)
{
    int result;

    DialogLayers* dialog = new DialogLayers(parent, layers, title, form_builder);
    result = dialog->exec();
    
    delete dialog;

    return result != 0;
}

void DialogLayers::closeEvent(QCloseEvent*)
{
    reject();
}

void DialogLayers::accept()
{
    _form->applyConfig();
    QDialog::accept();
}

void DialogLayers::revert()
{
    _form->revertConfig();
}
