#include "PackStream.h"

#include "Logs.h"
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QByteArray>

class PackStream::pimpl {
  public:
    QFile *file;
    QByteArray *buffer;
    QDataStream *stream;
};

PackStream::PackStream() : pv(new pimpl) {
    pv->file = NULL;
    pv->buffer = new QByteArray();
    pv->stream = new QDataStream(pv->buffer, QIODevice::WriteOnly);
    pv->stream->setVersion(QDataStream::Qt_5_4);
}

PackStream::PackStream(const PackStream *other) : pv(new pimpl) {
    pv->file = NULL;
    pv->buffer = new QByteArray();
    if (other->pv->file) {
        Logs::error("System") << "Try to read from a substream bound to a file: "
                              << other->pv->file->fileName().toStdString() << endl;
        pv->stream = new QDataStream(pv->buffer, QIODevice::ReadOnly);
    } else {
        pv->stream = new QDataStream(other->pv->buffer, QIODevice::ReadOnly);
    }
    pv->stream->setVersion(QDataStream::Qt_5_4);
}

PackStream::PackStream(const string &buffer_content) : pv(new pimpl) {
    pv->file = NULL;
    pv->buffer = new QByteArray(buffer_content.c_str(), buffer_content.size());
    pv->stream = new QDataStream(pv->buffer, QIODevice::ReadOnly);
    pv->stream->setVersion(QDataStream::Qt_5_4);
}

PackStream::~PackStream() {
    delete pv->buffer;
    delete pv->stream;
    if (pv->file) {
        delete pv->file;
    }
}

bool PackStream::bindToFile(const string &filepath, bool write) {
    if (not pv->file) {
        pv->file = new QFile(QString::fromStdString(filepath));
        if (not pv->file->open(write ? QIODevice::WriteOnly : QIODevice::ReadOnly)) {
            return false;
        }

        QDataStream *new_stream = new QDataStream(pv->file);
        if (new_stream) {
            delete pv->stream;
            pv->stream = new_stream;
            pv->stream->setVersion(QDataStream::Qt_5_4);
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
        *pv->stream << *value;
    }
}

void PackStream::write(const double *value) {
    if (value) {
        *pv->stream << *value;
    }
}

void PackStream::write(const char *value, int max_length) {
    if (value) {
        int length = qstrlen(value);
        *pv->stream << QString::fromUtf8(value, length > max_length ? max_length : length);
    }
}

void PackStream::write(const string &value) {
    *pv->stream << QString::fromStdString(value);
}

void PackStream::writeFromBuffer(const PackStream &other, bool prepend_size) {
    if (other.pv->file) {
        Logs::error("System") << "Try to write from a substream bound to a file: "
                              << other.pv->file->fileName().toStdString() << endl;
    } else {
        if (prepend_size) {
            int buffer_size = (int)other.pv->buffer->size();
            write(&buffer_size);
        }
        pv->stream->writeRawData(other.pv->buffer->data(), other.pv->buffer->size());
    }
}

string PackStream::getBuffer() {
    if (pv->file) {
        Logs::error("System") << "Try to get buffer on a stream bound to a file: " << pv->file->fileName().toStdString()
                              << endl;
        return "";
    } else {
        return pv->buffer->toStdString();
    }
}

void PackStream::read(int *value) {
    if (value and not pv->stream->atEnd()) {
        int output;
        *pv->stream >> output;
        *value = output;
    }
}

void PackStream::read(double *value) {
    if (value and not pv->stream->atEnd()) {
        double output;
        *pv->stream >> output;
        *value = output;
    }
}

void PackStream::read(char *value, int max_length) {
    if (value and not pv->stream->atEnd()) {
        QString output;
        *pv->stream >> output;
        QByteArray array = output.toUtf8();
        qstrncpy(value, array.constData(), max_length);
    }
}

string PackStream::readString() {
    if (not pv->stream->atEnd()) {
        QString output;
        *pv->stream >> output;
        return output.toStdString();
    } else {
        return "";
    }
}

void PackStream::skip(const int &value, int count) {
    pv->stream->skipRawData(sizeof(value) * count);
}

void PackStream::skip(const double &value, int count) {
    pv->stream->skipRawData(sizeof(value) * count);
}

void PackStream::skipBytes(int bytes) {
    pv->stream->skipRawData(bytes);
}
