#include "previewcolorgradation.h"

#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QColor>
#include <QPainter>
#include <QSlider>
#include <QScrollArea>
#include <QPushButton>
#include "Curve.h"
#include "baseform.h"
#include "tools.h"
#include "widgetcurveeditor.h"

/**************** Preview ****************/
PreviewColorGradation::PreviewColorGradation(QWidget* parent, ColorGradation* gradation, EnumColorGradationBand band) : QWidget(parent)
{
    this->gradation = gradation;
    this->band = band;
}

void PreviewColorGradation::paintEvent(QPaintEvent*)
{
    Curve curve;
    QPainter painter(this);
    int width = this->width();
    int height = this->height();

    switch (band)
    {
        case COLORGRADATIONBAND_RED:
            colorGradationGetRedCurve(gradation, &curve);
            break;
        case COLORGRADATIONBAND_GREEN:
            colorGradationGetGreenCurve(gradation, &curve);
            break;
        case COLORGRADATIONBAND_BLUE:
            colorGradationGetBlueCurve(gradation, &curve);
            break;
        default:
            break;
    }

    for (int x = 0; x < width; x++)
    {
        switch (band)
        {
            case COLORGRADATIONBAND_RED:
                painter.setPen(QColor::fromRgbF(curve.getValue((double)x / (double)width), 0.0, 0.0));
                break;
            case COLORGRADATIONBAND_GREEN:
                painter.setPen(QColor::fromRgbF(0.0, curve.getValue((double)x / (double)width), 0.0));
                break;
            case COLORGRADATIONBAND_BLUE:
                painter.setPen(QColor::fromRgbF(0.0, 0.0, curve.getValue((double)x / (double)width)));
                break;
            case COLORGRADATIONBAND_FINAL:
                painter.setPen(colorToQColor(colorGradationGet(gradation, (double)x / (double)width)));
                break;
        }
        painter.drawLine(x, 0, x, height - 1);
    }
}

void PreviewColorGradation::mouseReleaseEvent(QMouseEvent*)
{
    emit clicked();
}
