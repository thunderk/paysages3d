#include "CacheFile.h"

#include <QString>
#include "DataFile.h"

CacheFile::CacheFile(const std::string &module, const std::string &ext, const std::string &tag1, int tag2, int tag3, int tag4, int tag5, int tag6)
{
    filepath = QString("cache/%1-%2-%3-%4-%5-%6-%7.%8").arg(QString::fromStdString(module)).arg(QString::fromStdString(tag1)).arg(tag2).arg(tag3).arg(tag4).arg(tag5).arg(tag6).arg(QString::fromStdString(ext)).toStdString();
}

bool CacheFile::isReadable()
{
    FILE* f = fopen(filepath.c_str(), "rb");
    if (f)
    {
        fclose(f);
        return true;
    }
    else
    {
        std::string datapath = DataFile::findFile(filepath);
        if (datapath.empty())
        {
            return false;
        }
        else
        {
            FILE* f = fopen(datapath.c_str(), "rb");
            if (f)
            {
                fclose(f);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

bool CacheFile::isWritable()
{
    FILE* f = fopen("cache/.test", "wb");
    if (f)
    {
        fclose(f);
        return true;
    }
    else
    {
        return false;
    }
}

std::string CacheFile::getPath()
{
    std::string datapath = DataFile::findFile(filepath);
    if (datapath.empty())
    {
        return filepath;
    }
    else
    {
        return datapath;
    }
}
