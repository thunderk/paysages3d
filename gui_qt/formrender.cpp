#include "formrender.h"

#include <QFileDialog>
#include <QMessageBox>

#include "dialogrender.h"
#include "../lib_paysages/shared/functions.h"

/**************** Form ****************/
FormRender::FormRender(QWidget *parent) :
    BaseForm(parent),
    _quality(5),
    _width(800),
    _height(600)
{
    QPushButton* button;

    addInputInt("Quality", &_quality, 1, 10, 1, 1);
    addInputInt("Image width", &_width, 100, 2000, 10, 100);
    addInputInt("Image height", &_height, 100, 2000, 10, 100);

    button = addButton("Start new render");
    connect(button, SIGNAL(clicked()), this, SLOT(startRender()));
    button = addButton("Show last render");
    connect(button, SIGNAL(clicked()), this, SLOT(showRender()));
    button = addButton("Save last render");
    connect(button, SIGNAL(clicked()), this, SLOT(saveRender()));

    revertConfig();
}

void FormRender::startRender()
{
    DialogRender* dialog = new DialogRender(this);
    dialog->startRender(_quality, _width, _height);
 
    delete dialog;
}

void FormRender::showRender()
{
    DialogRender* dialog = new DialogRender(this);
    dialog->loadLastRender();
    
    delete dialog;
}

void FormRender::saveRender()
{
    QString filepath;
    
    filepath = QFileDialog::getSaveFileName(this, "Choose a filename to save the last render");
    if (!filepath.isNull())
    {
        renderSaveToFile((char*)filepath.toStdString().c_str());
        QMessageBox::information(this, "Message", "The picture " + filepath + " has been saved.");
    }
}
