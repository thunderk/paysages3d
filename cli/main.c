#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib_paysages/auto.h"
#include "../lib_paysages/main.h"
#include "../lib_paysages/render.h"
#include "../lib_paysages/scenery.h"

void startRender(Renderer* renderer, char* outputpath, RenderParams params)
{
    printf("\rRendering %s ...                   \n", outputpath);
    rendererStart(renderer, params);
    printf("\rSaving %s ...                      \n", outputpath);
    remove(outputpath);
    renderSaveToFile(renderer->render_area, outputpath);
}

void displayHelp()
{
    printf("Usage : paysages-cli [options]\n");
    printf("Options :\n");
    printf(" -h     Show this help\n");
    printf(" -f x   Saved file to load (str)\n");
    printf(" -n     Number of pictures in the sequence\n");
    printf(" -rw x  Render width (int)\n");
    printf(" -rh x  Render height (int)\n");
    printf(" -rq x  Render quality (int, 1 to 10)\n");
    printf(" -ra x  Render anti-aliasing (int, 1 to 4)\n");
    printf(" -di x  Day start time (float, 0.0 to 1.0)\n");
    printf(" -ds x  Day step time (float)\n");
}

void _previewUpdate(float progress)
{
    printf("\rProgress : %0.1f%%                         ", progress * 100.0);
    fflush(stdout);
}

int main(int argc, char** argv)
{
    Renderer renderer;
    char* conf_file_path = NULL;
    RenderParams conf_render_params = {800, 600, 1, 5};
    int conf_nb_pictures = 1;
    float conf_daytime_start = 0.4;
    float conf_daytime_step = 0.0;
    int outputcount;
    char outputpath[500];

    argc--;
    argv++;

    while (argc--)
    {
        if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0)
        {
            displayHelp();
            return 0;
        }
        else if (strcmp(*argv, "-f") == 0 || strcmp(*argv, "--file") == 0)
        {
            if (argc--)
            {
                conf_file_path = *(++argv);
            }
        }
        else if (strcmp(*argv, "-n") == 0 || strcmp(*argv, "--count") == 0)
        {
            if (argc--)
            {
                conf_nb_pictures = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-rw") == 0 || strcmp(*argv, "--width") == 0)
        {
            if (argc--)
            {
                conf_render_params.width = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-rh") == 0 || strcmp(*argv, "--height") == 0)
        {
            if (argc--)
            {
                conf_render_params.height = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-rq") == 0 || strcmp(*argv, "--quality") == 0)
        {
            if (argc--)
            {
                conf_render_params.quality = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-ra") == 0 || strcmp(*argv, "--antialias") == 0)
        {
            if (argc--)
            {
                conf_render_params.antialias = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-di") == 0 || strcmp(*argv, "--daystart") == 0)
        {
            if (argc--)
            {
                conf_daytime_start = atof(*(++argv));
            }
        }
        else if (strcmp(*argv, "-ds") == 0 || strcmp(*argv, "--daystep") == 0)
        {
            if (argc--)
            {
                conf_daytime_step = atof(*(++argv));
            }
        }

        argv++;
    }

    printf("Initializing ...\n");
    paysagesInit();
    
    if (conf_file_path)
    {
        paysagesLoad(conf_file_path);
    }

    renderer = sceneryCreateStandardRenderer();
    rendererSetPreviewCallbacks(&renderer, NULL, NULL, _previewUpdate);

    for (outputcount = 0; outputcount < conf_nb_pictures; outputcount++)
    {
        autoSetDaytimeFraction(conf_daytime_start);

        sprintf(outputpath, "output/pic%05d.png", outputcount);
        startRender(&renderer, outputpath, conf_render_params);

        conf_daytime_start += conf_daytime_step;
    }
    
    printf("Cleaning up ...\n");
    rendererDelete(&renderer);
    paysagesQuit();

    printf("\rDone.                         \n");

    return 0;
}
