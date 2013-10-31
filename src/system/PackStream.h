#ifndef PACKSTREAM_H
#define PACKSTREAM_H

#include "system_global.h"

#ifdef __cplusplus

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

    void bindToFile(const char* filepath, bool write=false);

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

extern "C" {
#endif

// Transitional C-API

#ifndef __cplusplus
typedef struct PackStream PackStream;
#endif

SYSTEMSHARED_EXPORT PackStream* packReadFile(const char* filepath);
SYSTEMSHARED_EXPORT PackStream* packWriteFile(const char* filepath);
SYSTEMSHARED_EXPORT void packCloseStream(PackStream* stream);

SYSTEMSHARED_EXPORT void packWriteDouble(PackStream* stream, double* value);
SYSTEMSHARED_EXPORT void packReadDouble(PackStream* stream, double* value);
SYSTEMSHARED_EXPORT void packWriteInt(PackStream* stream, int* value);
SYSTEMSHARED_EXPORT void packReadInt(PackStream* stream, int* value);
SYSTEMSHARED_EXPORT void packWriteString(PackStream* stream, char* value, int max_length);
SYSTEMSHARED_EXPORT void packReadString(PackStream* stream, char* value, int max_length);

#ifdef __cplusplus
}
#endif

#endif // PACKSTREAM_H
