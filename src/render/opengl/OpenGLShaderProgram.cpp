#include "OpenGLShaderProgram.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QDir>
#include "Texture2D.h"
#include "Texture3D.h"
#include "Texture4D.h"
#include "Color.h"

OpenGLShaderProgram::OpenGLShaderProgram(QString name, QOpenGLFunctions* functions):
    name(name), functions(functions)
{
    program = new QOpenGLShaderProgram();
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    delete program;
}

void OpenGLShaderProgram::addVertexSource(QString path)
{
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(":/shaders/%1.vert").arg(path));
}

void OpenGLShaderProgram::addFragmentSource(QString path)
{
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(":/shaders/%1.frag").arg(path));
}

void OpenGLShaderProgram::compile()
{
    if (not program->link())
    {
        qWarning() << "Error while compiling shader " << name << "\n" << program->log() << "\n";
    }
    else
    {
        qDebug() << "Shader " << name << " compilation output:\n" << program->log() << "\n";
    }
}

void OpenGLShaderProgram::updateCamera(const QVector3D& location, const QMatrix4x4& view)
{
    this->camera_location = location;
    this->view = view;
}

void OpenGLShaderProgram::updateWaterHeight(double height)
{
    this->water_height = height;
}

void OpenGLShaderProgram::updateSun(const QVector3D& direction, const QColor& color)
{
    this->sun_direction = direction;
    this->sun_color = color;
}

void OpenGLShaderProgram::addTexture(QString sampler_name, Texture2D* texture)
{
    GLuint texid;

    if (textures.contains(sampler_name))
    {
        texid = textures[sampler_name].second;
    }
    else
    {
        glGenTextures(1, &texid);
        textures[sampler_name] = QPair<int, unsigned int>(GL_TEXTURE_2D, texid);
    }

    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int sx, sy;
    texture->getSize(&sx, &sy);
    float* pixels = new float[sx * sy * 4];
    for (int x = 0; x < sx; x++)
    {
        for (int y = 0; y < sy; y++)
        {
            float* pixel = pixels + (y * sx + x) * 4;
            Color col = texture->getPixel(x, y);
            pixel[0] = (float)col.r;
            pixel[1] = (float)col.g;
            pixel[2] = (float)col.b;
            pixel[3] = (float)col.a;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sx, sy, 0, GL_RGBA, GL_FLOAT, pixels);
    delete[] pixels;
}

void OpenGLShaderProgram::addTexture(QString sampler_name, Texture3D* texture)
{
    GLuint texid;

    if (textures.contains(sampler_name))
    {
        texid = textures[sampler_name].second;
    }
    else
    {
        glGenTextures(1, &texid);
        textures[sampler_name] = QPair<int, unsigned int>(GL_TEXTURE_3D, texid);
    }

    glBindTexture(GL_TEXTURE_3D, texid);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int sx, sy, sz;
    texture->getSize(&sx, &sy, &sz);
    float* pixels = new float[sx * sy * sz * 4];
    for (int x = 0; x < sx; x++)
    {
        for (int y = 0; y < sy; y++)
        {
            for (int z = 0; z < sz; z++)
            {
                float* pixel = pixels + (z * (sx * sy) + y * sx + x) * 4;
                Color col = texture->getPixel(x, y, z);
                pixel[0] = (float)col.r;
                pixel[1] = (float)col.g;
                pixel[2] = (float)col.b;
                pixel[3] = (float)col.a;
            }
        }
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sx, sy, sz, 0, GL_RGBA, GL_FLOAT, pixels);
    delete[] pixels;
}

void OpenGLShaderProgram::addTexture(QString sampler_name, Texture4D* texture)
{
    GLuint texid;

    if (textures.contains(sampler_name))
    {
        texid = textures[sampler_name].second;
    }
    else
    {
        glGenTextures(1, &texid);
        textures[sampler_name] = QPair<int, unsigned int>(GL_TEXTURE_3D, texid);
    }

    glBindTexture(GL_TEXTURE_3D, texid);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int sx, sy, sz, sw;
    texture->getSize(&sx, &sy, &sz, &sw);
    float* pixels = new float[sx * sy * sz * sw * 4];
    for (int x = 0; x < sx; x++)
    {
        for (int y = 0; y < sy; y++)
        {
            for (int z = 0; z < sz; z++)
            {
                for (int w = 0; w < sw; w++)
                {
                    float* pixel = pixels + (w * (sx * sy * sz) + z * (sx * sy) + y * sx + x) * 4;
                    Color col = texture->getPixel(x, y, z, w);
                    pixel[0] = (float)col.r;
                    pixel[1] = (float)col.g;
                    pixel[2] = (float)col.b;
                    pixel[3] = (float)col.a;
                }
            }
        }
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sx, sy, sz * sw, 0, GL_RGBA, GL_FLOAT, pixels);
    delete[] pixels;
}

void OpenGLShaderProgram::bind()
{
    program->bind();

    // TODO Keep locations in cache

    int viewMatrix = program->uniformLocation("viewMatrix");
    if (viewMatrix >= 0)
    {
        program->setUniformValue(viewMatrix, view);
    }

    int cameraLocation = program->uniformLocation("cameraLocation");
    if (cameraLocation >= 0)
    {
        program->setUniformValue(cameraLocation, camera_location);
    }

    int waterHeight = program->uniformLocation("waterHeight");
    if (waterHeight >= 0)
    {
        program->setUniformValue(waterHeight, water_height);
    }

    int sunDirection = program->uniformLocation("sunDirection");
    if (sunDirection >= 0)
    {
        program->setUniformValue(sunDirection, sun_direction);
    }

    int sunColor = program->uniformLocation("sunColor");
    if (sunColor >= 0)
    {
        program->setUniformValue(sunColor, sun_color);
    }

    QMapIterator<QString, QPair<int, unsigned int> > iter(textures);
    int i = 0;
    while (iter.hasNext())
    {
        iter.next();
        int textureSampler = program->uniformLocation(iter.key());
        if (textureSampler >= 0)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(iter.value().first, iter.value().second);
            program->setUniformValue(textureSampler, i);
            i++;
        }
    }
}

void OpenGLShaderProgram::release()
{
    program->release();
}

void OpenGLShaderProgram::drawTriangles(float* vertices, int triangle_count)
{
    bind();

    GLuint vertex = program->attributeLocation("vertex");
    program->setAttributeArray(vertex, GL_FLOAT, vertices, 3);
    program->enableAttributeArray(vertex);

    glDrawArrays(GL_TRIANGLES, 0, triangle_count * 3);

    program->disableAttributeArray(vertex);

    release();
}

void OpenGLShaderProgram::drawTriangleStrip(float* vertices, int vertex_count)
{
    bind();

    GLuint vertex = program->attributeLocation("vertex");
    program->setAttributeArray(vertex, GL_FLOAT, vertices, 3);
    program->enableAttributeArray(vertex);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);

    program->disableAttributeArray(vertex);

    release();
}
