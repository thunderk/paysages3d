#include "system.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "IL/il.h"
#include "IL/ilu.h"

#ifndef NDEBUG
#define DEBUG_ONETHREAD 1
#endif

#ifdef WIN32
#include <windows.h>
#endif

static int _core_count = 1;

static int _getCoreCount()
{
    int core_count = 1;
#ifdef WIN32
    DWORD processAffinityMask;
    DWORD systemAffinityMask;

    if (GetProcessAffinityMask( GetCurrentProcess(),
                                &processAffinityMask,
                                &systemAffinityMask)){
        processAffinityMask = (processAffinityMask & 0x55555555)
            + (processAffinityMask >> 1 & 0x55555555);
        processAffinityMask = (processAffinityMask & 0x33333333)
            + (processAffinityMask >> 2 & 0x33333333);
        processAffinityMask = (processAffinityMask & 0x0f0f0f0f)
            + (processAffinityMask >> 4 & 0x0f0f0f0f);
        processAffinityMask = (processAffinityMask & 0x00ff00ff)
            + (processAffinityMask >> 8 & 0x00ff00ff);
        core_count          = (processAffinityMask & 0x0000ffff)
            + (processAffinityMask >>16 & 0x0000ffff);
    }
#endif
#ifdef _SC_NPROCESSORS_ONLN
    core_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
#endif
#ifdef DEBUG_ONETHREAD
    core_count = 1;
#endif
    return core_count;
}

void systemInit()
{
#ifndef GLIB_VERSION_2_32
    g_thread_init(NULL);
#endif
    _core_count = _getCoreCount();
    ilInit();
    iluInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);
}

int systemGetCoreCount()
{
    return _core_count;
}

int systemSavePictureFile(const char* filepath, PictureCallbackSavePixel callback_pixel, void* data, int width, int height)
{
    ILuint image_id;
    Color result;
    ILuint x, y;
    ILuint rgba;
    ILuint pixels[width * height];
    ILenum error;
    int error_count;

    ilGenImages(1, &image_id);
    ilBindImage(image_id);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            result = callback_pixel(data, x, y);
            rgba = colorTo32BitRGBA(&result);
            pixels[y * width + x] = rgba;
        }
    }

    ilTexImage((ILuint)width, (ILuint)height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, pixels);
    remove(filepath);
    ilSaveImage(filepath);

    ilDeleteImages(1, &image_id);

    error_count = 0;
    while ((error=ilGetError()) != IL_NO_ERROR)
    {
        fprintf(stderr, "IL ERROR : %s\n", iluErrorString(error));
        error_count++;
    }
    return !error_count;
}

int systemLoadPictureFile(const char* filepath, PictureCallbackLoadStarted callback_start, PictureCallbackLoadPixel callback_pixel, void* data)
{
    ILuint image_id;
    ILenum error;
    int error_count;
    int width, height;
    ILuint* pixels;
    int x, y;

    ilGenImages(1, &image_id);
    ilBindImage(image_id);

    if (ilLoadImage(filepath))
    {
        width = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        if (callback_start)
        {
            callback_start(data, width, height);
        }

        pixels = malloc(sizeof(ILuint) * width * height);
        ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixels);

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                callback_pixel(data, x, y, colorFrom32BitRGBA(pixels[y * width + x]));
            }
        }

        free(pixels);
    }

    error_count = 0;
    while ((error=ilGetError()) != IL_NO_ERROR)
    {
        fprintf(stderr, "IL ERROR : %s\n", iluErrorString(error));
        error_count++;
    }
    return !error_count;
}

int systemGetFileSize(const char* path)
{
    struct stat st;
    stat(path, &st);
    return st.st_size;
}
