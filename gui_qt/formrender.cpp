#include "formrender.h"

#include "dialogrender.h"

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
    DialogRender* dialog = new DialogRender(this, _quality, _width, _height);
    dialog->exec();

    delete dialog;
}

void FormRender::showRender()
{

}

void FormRender::saveRender()
{

}
