#include "FileSystem.h"

#include <QDir>
#include <QFileInfo>
#include <cstdio>

string FileSystem::getTempFile(const string &filename) {
    return QDir::temp().filePath(QString::fromStdString(filename)).toStdString();
}

bool FileSystem::isFile(const string &filepath) {
    return QFileInfo(QString::fromStdString(filepath)).exists();
}

bool FileSystem::removeFile(const string &filepath) {
    if (FileSystem::isFile(filepath)) {
        remove(filepath.c_str());
        return true;
    } else {
        return false;
    }
}
