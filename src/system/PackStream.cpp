#include "PackStream.h"

#include <QFile>
#include <QDataStream>
#include <QString>

PackStream::PackStream()
{
    file = NULL;
    stream = NULL;
}

PackStream::~PackStream()
{
    if (stream)
    {
        delete stream;
    }
    if (file)
    {
        delete file;
    }
}

bool PackStream::bindToFile(const char* filepath, bool write)
{
    if (not file and not stream)
    {
        file = new QFile(filepath);
        if (not file->open(write ? QIODevice::WriteOnly : QIODevice::ReadOnly))
        {
            return false;
        }

        stream = new QDataStream(file);
    }
    return stream != NULL;
}

void PackStream::write(int* value)
{
    if (stream and value)
    {
        *stream << *value;
    }
}

void PackStream::write(double* value)
{
    if (stream and value)
    {
        *stream << *value;
    }
}

void PackStream::write(char* value, int max_length)
{
    if (stream and value)
    {
        int length = qstrlen(value);
        *stream << QString::fromUtf8(value, length > max_length ? max_length : length);
    }
}

void PackStream::write(QString value)
{
    if (stream)
    {
        *stream << value;
    }
}

void PackStream::read(int* value)
{
    if (stream and value and not stream->atEnd())
    {
        int output;
        *stream >> output;
        *value = output;
    }
}

void PackStream::read(double* value)
{
    if (stream and value and not stream->atEnd())
    {
        double output;
        *stream >> output;
        *value = output;
    }
}

void PackStream::read(char* value, int max_length)
{
    if (stream and value and not stream->atEnd())
    {
        QString output;
        *stream >> output;
        QByteArray array = output.toUtf8();
        qstrncpy(value, array.constData(), max_length);
    }
}

QString PackStream::readString()
{
    if (stream and not stream->atEnd())
    {
        QString output;
        *stream >> output;
        return output;
    }
    else
    {
        return QString();
    }
}
