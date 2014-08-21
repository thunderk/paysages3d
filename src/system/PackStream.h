#ifndef PACKSTREAM_H
#define PACKSTREAM_H

#include "system_global.h"

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

    bool bindToFile(const std::string &filepath, bool write=false);

    void write(const int *value);
    void write(const double *value);
    void write(const char *value, const int max_length);
    void write(const std::string &value);

    void read(int* value);
    void read(double* value);
    void read(char* value, int max_length);
    std::string readString();

    void skip(const int &value, int count=1);
    void skip(const double &value, int count=1);

private:
    QFile* file;
    QDataStream* stream;
};

}
}

#endif // PACKSTREAM_H
