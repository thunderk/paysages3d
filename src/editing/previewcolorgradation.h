#ifndef _PAYSAGES_QT_PREVIEWCOLORGRADATION_H_
#define _PAYSAGES_QT_PREVIEWCOLORGRADATION_H_

#include <QWidget>

#include "tools/color.h"

typedef enum
{
    COLORGRADATIONBAND_RED,
    COLORGRADATIONBAND_GREEN,
    COLORGRADATIONBAND_BLUE,
    COLORGRADATIONBAND_FINAL
} EnumColorGradationBand;

class PreviewColorGradation:public QWidget
{
    Q_OBJECT
public:
    PreviewColorGradation(QWidget* parent, ColorGradation* gradation, EnumColorGradationBand band);
    void paintEvent(QPaintEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void clicked();

private:
    ColorGradation* gradation;
    EnumColorGradationBand band;
};

#endif
