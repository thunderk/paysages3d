#ifndef _GUI_QT_FORMWATER_H_
#define _GUI_QT_FORMWATER_H_

#include <QWidget>
#include "preview.h"
#include "baseform.h"

class FormWater : public BaseForm
{
    Q_OBJECT

public:
    explicit FormWater(QWidget *parent = 0);

public slots:
    virtual void revertConfig();

private:
    Preview* previewCoverage;
    Preview* previewColor;
};

#endif // _GUI_QT_FORMWATER_H_
