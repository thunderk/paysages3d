#include "FileSystem.h"

#include <QDir>
#include <QFileInfo>
#include <cstdio>

std::string FileSystem::getTempFile(const std::string &filename) {
    return QDir::temp().filePath(QString::fromStdString(filename)).toStdString();
}

bool FileSystem::isFile(const std::string &filepath) {
    return QFileInfo(QString::fromStdString(filepath)).exists();
}

bool FileSystem::removeFile(const std::string &filepath) {
    if (FileSystem::isFile(filepath)) {
        remove(filepath.c_str());
    }
}
