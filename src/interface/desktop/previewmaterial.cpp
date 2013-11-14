#include "previewmaterial.h"

#include <cmath>
#include <QPainter>
#include "tools.h"
#include "SoftwareRenderer.h"
#include "BasePreview.h"

#include "tools/lighting.h"
#include "tools/color.h"
#include "CameraDefinition.h"

/***** Shared renderer *****/
MaterialPreviewRenderer::MaterialPreviewRenderer(SurfaceMaterial* material)
{
    _light.color.r = 3.0;
    _light.color.g = 3.0;
    _light.color.b = 3.0;
    _light.direction.x = -0.5;
    _light.direction.y = -0.5;
    _light.direction.z = -0.5;
    _light.direction = v3Normalize(_light.direction);
    _light.altered = 0;
    _light.reflection = 1.0;

    _material = material;

    render_camera->setLocation(Vector3(0.0, 0.0, 10.0));

    _color_profile = colorProfileCreate();
    colorProfileSetToneMapping(_color_profile, TONE_MAPPING_UNCHARTED, 1.0);
}

MaterialPreviewRenderer::~MaterialPreviewRenderer()
{
    colorProfileDelete(_color_profile);
}

void MaterialPreviewRenderer::bindEvent(BasePreview* preview)
{
    preview->configScaling(0.05, 2.0, 0.05, 2.0);
}

Color MaterialPreviewRenderer::getColor2D(double x, double y, double)
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
        color = lightingApplyOneLight(&_light, getCameraLocation(this, point), point, point, _material);
        if (dist > 0.95)
        {
            color.a = (1.0 - dist) / 0.05;
        }
        return colorProfileApply(_color_profile, color);
    }
}

/***** Small static preview *****/

SmallMaterialPreview::SmallMaterialPreview(QWidget* parent, SurfaceMaterial* material):
QWidget(parent),
_renderer(material)
{
}

Color SmallMaterialPreview::getColor(double x, double y)
{
    return _renderer.getColor2D(x, y, 1.0);
}

void SmallMaterialPreview::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    int width = this->width();
    int height = this->height();
    double factor, dx, dy;

    if (width > height)
    {
        factor = 2.0 / (double) height;
    }
    else
    {
        factor = 2.0 / (double) width;
    }
    dx = factor * (double) width / 2.0;
    dy = factor * (double) height / 2.0;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            painter.setPen(colorToQColor(_renderer.getColor2D((double) x * factor - dx, (double) y * factor - dy, 1.0)));
            painter.drawPoint(x, y);
        }
    }
}
