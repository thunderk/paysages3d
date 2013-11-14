#ifndef _PAYSAGES_QT_INPUTNOISE_H_
#define _PAYSAGES_QT_INPUTNOISE_H_

#include "desktop_global.h"

#include "baseinput.h"

#include <QObject>
class QWidget;

class InputNoise:public BaseInput
{
    Q_OBJECT

public:
    InputNoise(QWidget* form, QString label, NoiseGenerator* value);

public slots:
    virtual void updatePreview();
    virtual void applyValue();
    virtual void revert();

private slots:
    void editNoise();

private:
    NoiseGenerator* _value;
};

#endif
