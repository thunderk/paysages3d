#ifndef _PAYSAGES_QT_DIALOGCOLORGRADATION_H_
#define _PAYSAGES_QT_DIALOGCOLORGRADATION_H_

#include <QDialog>
#include "baseform.h"
#include "widgetcurveeditor.h"
#include "previewcolorgradation.h"

#include "../lib_paysages/color.h"
#include "../lib_paysages/curve.h"

class DialogColorGradation : public QDialog
{
    Q_OBJECT
public:
    explicit DialogColorGradation(QWidget* parent, ColorGradation* gradation);
    ~DialogColorGradation();

    static bool getGradation(QWidget* parent, ColorGradation* gradation);

public slots:
    virtual void accept();
    void revert();
    void selectRed();
    void selectGreen();
    void selectBlue();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    void revertToCurrent();

private:
    ColorGradation* _base;
    ColorGradation* _current;
    Curve* _curve;
    WidgetCurveEditor* _curve_editor;
    PreviewColorGradation* _preview_red;
    PreviewColorGradation* _preview_green;
    PreviewColorGradation* _preview_blue;
    PreviewColorGradation* _preview_final;
};

#endif
