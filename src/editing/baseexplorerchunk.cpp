#include "baseexplorerchunk.h"

BaseExplorerChunk::BaseExplorerChunk(Renderer* renderer)
{
    _renderer = renderer;
    _color_profile = colorProfileCreate();

    priority = 0.0;
    _reset_needed = false;

    _texture = new QImage(1, 1, QImage::Format_ARGB32);
    _texture_id = 0;
    _texture_changed = false;
    _texture_current_size = 0;
    _texture_max_size = 0;
}

BaseExplorerChunk::~BaseExplorerChunk()
{
    _lock_data.lock();
    colorProfileDelete(_color_profile);
    delete _texture;
    _lock_data.unlock();
}

bool BaseExplorerChunk::maintain()
{
    bool subchanged;

    _lock_data.lock();
    if (_reset_needed)
    {
        _reset_needed = false;
        _texture_current_size = 0;
        onResetEvent();
    }
    _lock_data.unlock();

    subchanged = onMaintainEvent();

    // Improve texture resolution
    if (_texture_current_size < _texture_max_size)
    {
        int new_texture_size = _texture_current_size ? _texture_current_size * 2 : 1;
        QImage* new_image = new QImage(_texture->scaled(new_texture_size + 1, new_texture_size + 1, Qt::IgnoreAspectRatio, Qt::FastTransformation));
        for (int j = 0; j <= new_texture_size; j++)
        {
            for (int i = 0; i <= new_texture_size; i++)
            {
                if (_texture_current_size <= 1 || i % 2 != 0 || j % 2 != 0)
                {
                    Color color = getTextureColor((double)i / (double)new_texture_size, 1.0 - (double)j / (double)new_texture_size);
                    color = colorProfileApply(_color_profile, color);
                    colorNormalize(&color);
                    new_image->setPixel(i, j, colorTo32BitBGRA(&color));
                }
            }
        }

        _lock_data.lock();
        delete _texture;
        _texture = new_image;
        _texture_current_size = new_texture_size;
        _texture_changed = true;
        _lock_data.unlock();

        /*if (_texture_current_size < 4 && _texture_current_size < _texture_max_size)
        {
            maintain();
        }*/

        return true;
    }
    else
    {
        return subchanged;
    }
}

void BaseExplorerChunk::updatePriority(CameraDefinition* camera)
{
    if (_reset_needed || (_texture_max_size > 1 && _texture_current_size <= 1))
    {
        priority = 1000.0;
    }
    else if (_texture_current_size == _texture_max_size)
    {
        priority = -1000.0;
    }
    else
    {
        priority = getDisplayedSizeHint(camera) - _texture_current_size;
    }
    onCameraEvent(camera);
}

void BaseExplorerChunk::render(QGLWidget* widget)
{
    // Put texture in place
    _lock_data.lock();
    if (_texture_changed)
    {
        _texture_changed = false;
        if (_texture_id)
        {
            widget->deleteTexture(_texture_id);
        }
        // TODO Only do the scale if not power-of-two textures are unsupported by GPU
        _texture_id = widget->bindTexture(_texture->scaled(_texture_current_size, _texture_current_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        //_texture_id = widget->bindTexture(*_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _texture_id);
    }
    _lock_data.unlock();

    // Delegate poly rendering to subclass
    if (!_reset_needed)
    {
        onRenderEvent(widget);
    }
}

void BaseExplorerChunk::askReset()
{
    _reset_needed = true;
}

void BaseExplorerChunk::setMaxTextureSize(int size)
{
    _texture_max_size = size;
}

void BaseExplorerChunk::onCameraEvent(CameraDefinition*)
{
}

void BaseExplorerChunk::onResetEvent()
{
}

bool BaseExplorerChunk::onMaintainEvent()
{
    return false;
}

void BaseExplorerChunk::onRenderEvent(QGLWidget*)
{
}

double BaseExplorerChunk::getDisplayedSizeHint(CameraDefinition*)
{
    return 0.0;
}

Color BaseExplorerChunk::getTextureColor(double, double)
{
    return COLOR_TRANSPARENT;
}
