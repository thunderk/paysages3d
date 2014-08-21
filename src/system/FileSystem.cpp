#include "FileSystem.h"

#include <QDir>

std::string FileSystem::getTempFile(const std::string &filename)
{
    return QDir::temp().filePath(QString::fromStdString(filename)).toStdString();
}
