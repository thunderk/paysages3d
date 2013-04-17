#ifndef _PAYSAGES_QT_INPUTNOISE_H_
#define _PAYSAGES_QT_INPUTNOISE_H_

#include <QWidget>
#include "baseinput.h"

#include "rendering/noise.h"

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
