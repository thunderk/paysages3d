#include "formrender.h"

#include "dialogrender.h"

/**************** Form ****************/
FormRender::FormRender(QWidget *parent) :
    BaseForm(parent),
    _quality(5)
{
    QPushButton* button;

    addInputInt("Quality", &_quality, 1, 10, 1, 1);

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
    DialogRender* dialog = new DialogRender();
    dialog->exec();

    delete dialog;
}

void FormRender::showRender()
{

}

void FormRender::saveRender()
{

}
