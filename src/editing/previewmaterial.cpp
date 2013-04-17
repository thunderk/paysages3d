#include "previewmaterial.h"

#include <math.h>
#include <QPainter>
#include "tools.h"

#include "rendering/tools/lighting.h"
#include "rendering/tools/color.h"

/***** Small static preview *****/

SmallMaterialPreview::SmallMaterialPreview(QWidget* parent, SurfaceMaterial* material) : QWidget(parent)
{
    _light.color = COLOR_WHITE;
    _light.direction.x = -0.5;
    _light.direction.y = -0.5;
    _light.direction.z = -0.5;
    _light.direction = v3Normalize(_light.direction);
    _light.altered = 0;
    _light.reflection = 1.0;

    _material = material;

    _renderer = rendererCreate();
    _renderer->render_camera.location.x = 0.0;
    _renderer->render_camera.location.x = 0.0;
    _renderer->render_camera.location.z = 10.0;
}

SmallMaterialPreview::~SmallMaterialPreview()
{
    rendererDelete(_renderer);
}

Color SmallMaterialPreview::getColor(double x, double y)
{
    double dist = sqrt(x * x + y * y);
    Vector3 point;
    Color color;

    if (dist >= 1.0)
    {
        return COLOR_TRANSPARENT;
    }
    else
    {
        point.x = x;
        point.y = -y;
        if (dist == 0)
        {
            point.z = 1.0;
        }
        else
        {
            point.z = fabs(acos(dist));
        }

        point = v3Normalize(point);
        color = lightingApplyOneLight(&_light, _renderer->getCameraLocation(_renderer, point), point, point, _material);
        if (dist > 0.95)
        {
            color.a = (1.0 - dist) / 0.05;
        }
        return color;
    }
}

void SmallMaterialPreview::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    int width = this->width();
    int height = this->height();
    double factor, dx, dy;

    if (width > height)
    {
        factor = 2.0 / (double)height;
    }
    else
    {
        factor = 2.0 / (double)width;
    }
    dx = factor * (double)width / 2.0;
    dy = factor * (double)height / 2.0;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            painter.setPen(colorToQColor(getColor((double)x * factor - dx, (double)y * factor - dy)));
            painter.drawPoint(x, y);
        }
    }
}

/***** Large dynamic preview *****/

PreviewMaterial::PreviewMaterial(QWidget* parent, SurfaceMaterial* material) : BasePreview(parent)
{
    _small = new SmallMaterialPreview(this, material);
    _small->hide();

    configScaling(0.05, 2.0, 0.05, 2.0);
}

PreviewMaterial::~PreviewMaterial()
{
    delete _small;
}

Color PreviewMaterial::getColor(double x, double y)
{
    return _small->getColor(x, y);
}
