#include "main.h"

#include <cstdio>
#include <cstdlib>

#include "tools/data.h"

void paysagesInit()
{
    if (!dataInit())
    {
        /* TODO Add error callback (for interface) */
        fprintf(stderr, "ERROR : Can't locate data files.\n");
        exit(1);
    }
}

void paysagesQuit()
{
}
