#ifndef PACKSTREAM_H
#define PACKSTREAM_H

#include "system_global.h"

class QFile;
class QDataStream;
class QByteArray;

namespace paysages {
namespace system {

/*!
 * \brief Data (de)serialization in files or streams.
 */
class SYSTEMSHARED_EXPORT PackStream {
  public:
    PackStream();
    ~PackStream();

    /**
     * Open a reading stream for another stream.
     *
     * The other stream must not have been bound to a file (still a memory buffer).
     */
    PackStream(const PackStream *other);

    /**
     * Open a reading stream on a buffer content.
     */
    PackStream(const std::string &buffer_content);

    bool bindToFile(const std::string &filepath, bool write = false);

    void write(const int *value);
    void write(const double *value);
    void write(const char *value, const int max_length);
    void write(const std::string &value);

    /**
     * Write the contents of another stream into this one.
     *
     * The other stream must not have been bound to a file (still a memory buffer).
     *
     * If *prepend_size* is true, an integer will be written on front with the number of bytes written.
     */
    void writeFromBuffer(const PackStream &other, bool prepend_size = false);

    /**
     * Get the contents of the memory buffer, if this stream is not bound to a file.
     */
    std::string getBuffer();

    void read(int *value);
    void read(double *value);
    void read(char *value, int max_length);
    std::string readString();

    void skip(const int &value, int count = 1);
    void skip(const double &value, int count = 1);
    void skipBytes(int bytes);

  private:
    QFile *file;
    QByteArray *buffer;
    QDataStream *stream;
};
}
}

#endif // PACKSTREAM_H
