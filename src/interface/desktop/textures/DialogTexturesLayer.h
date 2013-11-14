#ifndef DIALOGTEXTURESLAYER_H
#define DIALOGTEXTURESLAYER_H

#include <QDialog>
#include "textures/public.h"

namespace Ui {
class DialogTexturesLayer;
}

class DialogTexturesLayer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTexturesLayer(QWidget* parent, TexturesDefinition* textures, int layer);
    ~DialogTexturesLayer();

private:
    Ui::DialogTexturesLayer *ui;

    int layer;
    TexturesDefinition* original;
    TexturesDefinition* modified;
};

#endif // DIALOGTEXTURESLAYER_H
