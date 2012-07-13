#ifndef _PAYSAGES_QT_INPUTHEIGHTMAP_H_
#define _PAYSAGES_QT_INPUTHEIGHTMAP_H_

#include <QWidget>
#include "baseinput.h"

#include "../lib_paysages/heightmap.h"

class InputHeightMap:public BaseInput
{
    Q_OBJECT

public:
    InputHeightMap(QWidget* form, QString label, HeightMap* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void editHeightMap();

private:
    HeightMap* _value;
};

#endif
