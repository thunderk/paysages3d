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

void PackStream::bindToFile(const char* filepath, bool write)
{
    if (not file and not stream)
    {
        file = new QFile(filepath);
        file->open(write ? QIODevice::WriteOnly : QIODevice::ReadOnly);

        stream = new QDataStream(file);
    }
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

// Transitional C-API

PackStream* packReadFile(const char* filepath)
{
    PackStream* result = new PackStream();
    result->bindToFile(filepath, false);
    return result;
}

PackStream* packWriteFile(const char* filepath)
{
    PackStream* result = new PackStream();
    result->bindToFile(filepath, true);
    return result;
}

void packCloseStream(PackStream* stream)
{
    delete stream;
}

void packWriteDouble(PackStream* stream, double* value)
{
    stream->write(value);
}

void packReadDouble(PackStream* stream, double* value)
{
    stream->read(value);
}

void packWriteInt(PackStream* stream, int* value)
{
    stream->write(value);
}

void packReadInt(PackStream* stream, int* value)
{
    stream->read(value);
}

void packWriteString(PackStream* stream, char* value, int max_length)
{
    stream->write(value, max_length);
}

void packReadString(PackStream* stream, char* value, int max_length)
{
    stream->read(value, max_length);
}
