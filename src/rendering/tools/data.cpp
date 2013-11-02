#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* _datapath = NULL;

static int _tryDataPath(const char* path)
{
    char* buffer;

    buffer = (char*)malloc(sizeof (char) * (strlen(path) + 30));
    strcpy(buffer, path);
    strcat(buffer, "/.paysages_data");

    FILE* f = fopen(buffer, "r");
    free(buffer);
    if (f)
    {
        _datapath = path;
        fclose(f);
        return 1;
    }
    else
    {
        return 0;
    }
}

int dataInit()
{
    return _tryDataPath("./data") || _tryDataPath("/usr/share/paysages3d");
}
