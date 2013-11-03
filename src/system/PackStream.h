#ifndef PACKSTREAM_H
#define PACKSTREAM_H

#include "system_global.h"
#include <QString>
class QFile;
class QDataStream;

namespace paysages
{
namespace system
{

/*!
 * \brief Data (de)serialization in files or streams.
 */
class SYSTEMSHARED_EXPORT PackStream
{
public:
    PackStream();
    ~PackStream();

    bool bindToFile(const char* filepath, bool write=false);

    void write(int* value);
    void write(double* value);
    void write(char* value, int max_length);
    void write(QString value);

    void read(int* value);
    void read(double* value);
    void read(char* value, int max_length);
    QString readString();

private:
    QFile* file;
    QDataStream* stream;
};

}
}

#endif // PACKSTREAM_H
