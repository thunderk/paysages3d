#include "PackStream.h"

#include "Logs.h"
#include <QFile>
#include <QDataStream>
#include <QString>

PackStream::PackStream() {
    file = NULL;
    buffer = new QByteArray();
    stream = new QDataStream(buffer, QIODevice::WriteOnly);
    stream->setVersion(QDataStream::Qt_5_2);
}

PackStream::~PackStream() {
    delete buffer;
    delete stream;
    if (file) {
        delete file;
    }
}

PackStream::PackStream(const PackStream *other) {
    file = NULL;
    buffer = new QByteArray();
    if (other->file) {
        Logs::error() << "Try to read from a substream bound to a file: " << other->file->fileName().toStdString()
                      << endl;
        stream = new QDataStream(buffer, QIODevice::ReadOnly);
    } else {
        stream = new QDataStream(other->buffer, QIODevice::ReadOnly);
    }
    stream->setVersion(QDataStream::Qt_5_2);
}

PackStream::PackStream(const string &buffer_content) {
    file = NULL;
    buffer = new QByteArray(buffer_content.c_str(), buffer_content.size());
    stream = new QDataStream(buffer, QIODevice::ReadOnly);
    stream->setVersion(QDataStream::Qt_5_2);
}

bool PackStream::bindToFile(const string &filepath, bool write) {
    if (not file) {
        file = new QFile(QString::fromStdString(filepath));
        if (not file->open(write ? QIODevice::WriteOnly : QIODevice::ReadOnly)) {
            return false;
        }

        QDataStream *new_stream = new QDataStream(file);
        if (new_stream) {
            delete stream;
            stream = new_stream;
            stream->setVersion(QDataStream::Qt_5_2);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void PackStream::write(const int *value) {
    if (value) {
        *stream << *value;
    }
}

void PackStream::write(const double *value) {
    if (value) {
        *stream << *value;
    }
}

void PackStream::write(const char *value, int max_length) {
    if (value) {
        int length = qstrlen(value);
        *stream << QString::fromUtf8(value, length > max_length ? max_length : length);
    }
}

void PackStream::write(const string &value) {
    *stream << QString::fromStdString(value);
}

void PackStream::writeFromBuffer(const PackStream &other, bool prepend_size) {
    if (other.file) {
        Logs::error() << "Try to write from a substream bound to a file: " << other.file->fileName().toStdString()
                      << endl;
    } else {
        if (prepend_size) {
            int buffer_size = (int)other.buffer->size();
            write(&buffer_size);
        }
        stream->writeRawData(other.buffer->data(), other.buffer->size());
    }
}

string PackStream::getBuffer() {
    if (file) {
        Logs::error() << "Try to get buffer on a stream bound to a file: " << file->fileName().toStdString() << endl;
        return "";
    } else {
        return buffer->toStdString();
    }
}

void PackStream::read(int *value) {
    if (value and not stream->atEnd()) {
        int output;
        *stream >> output;
        *value = output;
    }
}

void PackStream::read(double *value) {
    if (value and not stream->atEnd()) {
        double output;
        *stream >> output;
        *value = output;
    }
}

void PackStream::read(char *value, int max_length) {
    if (value and not stream->atEnd()) {
        QString output;
        *stream >> output;
        QByteArray array = output.toUtf8();
        qstrncpy(value, array.constData(), max_length);
    }
}

string PackStream::readString() {
    if (not stream->atEnd()) {
        QString output;
        *stream >> output;
        return output.toStdString();
    } else {
        return "";
    }
}

void PackStream::skip(const int &value, int count) {
    stream->skipRawData(sizeof(value) * count);
}

void PackStream::skip(const double &value, int count) {
    stream->skipRawData(sizeof(value) * count);
}

void PackStream::skipBytes(int bytes) {
    stream->skipRawData(bytes);
}
