#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib_paysages/shared/functions.h"

void startRender(char* outputpath)
{
    printf("\rRendering %s ...                   \n", outputpath);
    autoRenderSceneTwoPass(0);
    printf("\rSaving %s ...                      \n", outputpath);
    remove(outputpath);
    renderSaveToFile(outputpath);
}

void displayHelp()
{
    printf("Usage : TODO\n");
}

void _previewUpdate(double progress)
{
    printf("\rProgress : %0.1f%%                         ", progress * 100.0);
    fflush(stdout);
}

int main(int argc, char** argv)
{
    int conf_render_width = 800;
    int conf_render_height = 600;
    int conf_render_quality = 5;
    int conf_nb_pictures = 1;
    double conf_daytime_start = 0.4;
    double conf_daytime_step = 0.0;
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
                conf_render_width = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-rh") == 0 || strcmp(*argv, "--height") == 0)
        {
            if (argc--)
            {
                conf_render_height = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "-rq") == 0 || strcmp(*argv, "--quality") == 0)
        {
            if (argc--)
            {
                conf_render_quality = atoi(*(++argv));
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
    
    renderSetSize(conf_render_width, conf_render_height);
    renderSetQuality(conf_render_quality);
    renderSetPreviewCallbacks(NULL, NULL, NULL, _previewUpdate);
    
    for (outputcount = 0; outputcount < conf_nb_pictures; outputcount++)
    {
        autoSetDaytimeFraction(conf_daytime_start);
        
        sprintf(outputpath, "output/pic%05d.png", outputcount);
        startRender(outputpath);
        
        conf_daytime_start += conf_daytime_step;
    }
    
    printf("\rDone.                         \n");
    
    return 0;
}
