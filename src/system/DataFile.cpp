#include "DataFile.h"

#include "Logs.h"
#include <QDir>

string DataFile::findFile(const string &relpath) {
    if (dataDir.empty()) {
        dataDir = initDataDir();
    }

    QDir dir(QString::fromStdString(dataDir));
    if (dir.exists(QString::fromStdString(relpath))) {
        return dir.absoluteFilePath(QString::fromStdString(relpath)).toStdString();
    } else {
        return "";
    }
}

string DataFile::findDir(const string &relpath) {
    return findFile(relpath);
}

bool DataFile::tryDataDir(const QDir &dir) {
    Logs::debug() << "[System] Try data dir " << dir.absolutePath().toStdString() << endl;
    return dir.exists("data/.paysages_data");
}

string DataFile::locateDataDir() {
    QDir dir = QDir::current();
    int i = 0;

    // TODO /usr/share/paysages3d/

    while (i++ < 100 and not dir.isRoot()) {
        if (tryDataDir(dir)) {
            return dir.absolutePath().toStdString();
        }

        QDir dir2(dir.absoluteFilePath("paysages3d"));
        if (tryDataDir(dir2)) {
            return dir2.absolutePath().toStdString();
        }

        dir = QDir(QDir(dir.absoluteFilePath("..")).canonicalPath());
    }

    return "";
}

string DataFile::initDataDir() {
    string parent = locateDataDir();
    if (parent.empty()) {
        Logs::warning() << "[System] Data files not found" << endl;
        return parent;
    } else {
        string result = QDir(QString::fromStdString(parent)).absoluteFilePath("data").toStdString();
        Logs::debug() << "[System] Data files found : " << result << endl;
        return result;
    }
}

string DataFile::dataDir;
