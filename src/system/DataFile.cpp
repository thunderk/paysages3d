#include "DataFile.h"

#include "Logs.h"
#include <QDir>

std::string DataFile::findFile(const std::string &relpath)
{
    QDir dir(QString::fromStdString(dataDir));
    if (dir.exists(QString::fromStdString(relpath)))
    {
        return dir.absoluteFilePath(QString::fromStdString(relpath)).toStdString();
    }
    else
    {
        return "";
    }
}

std::string DataFile::findDir(const std::string &relpath)
{
    return findFile(relpath);
}

bool DataFile::tryDataDir(const QDir &dir)
{
    logDebug("[System] Try data dir %s", dir.absolutePath().toLocal8Bit().data());
    return dir.exists("data/.paysages_data");
}

std::string DataFile::locateDataDir()
{
    QDir dir = QDir::current();
    int i = 0;

    // TODO /usr/share/paysages3d/

    while (i++ < 100 and not dir.isRoot())
    {
        if (tryDataDir(dir))
        {
            return dir.absolutePath().toStdString();
        }

        QDir dir2(dir.absoluteFilePath("paysages3d"));
        if (tryDataDir(dir2))
        {
            return dir2.absolutePath().toStdString();
        }

        dir = QDir(QDir(dir.absoluteFilePath("..")).canonicalPath());
    }

    return "";
}

std::string DataFile::initDataDir()
{
    std::string parent = locateDataDir();
    if (parent.empty())
    {
        logWarning("[System] Data files not found");
        return parent;
    }
    else
    {
        std::string result = QDir(QString::fromStdString(parent)).absoluteFilePath("data").toStdString();
        logDebug("[System] Data files found : %s", result.c_str());
        return result;
    }
}

std::string DataFile::dataDir = DataFile::initDataDir();
