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

    void write(const int *value);
    void write(const double *value);
    void write(const char *value, const int max_length);
    void write(const QString &value);

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
