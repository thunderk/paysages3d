#ifndef _PAYSAGES_QT_DIALOGCOLORGRADATION_H_
#define _PAYSAGES_QT_DIALOGCOLORGRADATION_H_

#include <QDialog>
#include <QPushButton>
#include "widgetcurveeditor.h"
#include "previewcolorgradation.h"

#include "../lib_paysages/tools/color.h"
#include "../lib_paysages/tools/curve.h"

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
    void updateColors();

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
    int _selected;
    QPushButton* _button_accept;
    QPushButton* _button_revert;
    QPushButton* _button_cancel;
};

#endif
