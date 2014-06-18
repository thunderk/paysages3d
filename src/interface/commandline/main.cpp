#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "CameraDefinition.h"
#include "AtmosphereDefinition.h"
#include "SoftwareCanvasRenderer.h"
#include "Scenery.h"
#include "RenderConfig.h"

void startRender(SoftwareRenderer *renderer, char *outputpath, const RenderConfig &params)
{
    /*printf("\rRendering %s ...                   \n", outputpath);
    renderer->start(params);
    printf("\rSaving %s ...                      \n", outputpath);
    remove(outputpath);
    renderer->render_area->saveToFile(outputpath);*/
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
    printf(" -di x  Day start time (double, 0.0 to 1.0)\n");
    printf(" -ds x  Day step time (double)\n");
    printf(" -cx x  Camera X step (double)\n");
    printf(" -cy y  Camera Y step (double)\n");
    printf(" -cz z  Camera Z step (double)\n");
}

void _previewUpdate(double progress)
{
    printf("\rProgress : %0.1f%%                         ", progress * 100.0);
    fflush(stdout);
}

int main(int argc, char** argv)
{
    SoftwareCanvasRenderer* renderer;
    char* conf_file_path = NULL;
    RenderConfig conf_render_params(800, 600, 1, 5);
    int conf_first_picture = 0;
    int conf_nb_pictures = 1;
    double conf_daytime_start = 0.4;
    double conf_daytime_step = 0.0;
    double conf_camera_step_x = 0.0;
    double conf_camera_step_y = 0.0;
    double conf_camera_step_z = 0.0;
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
        else if (strcmp(*argv, "-s") == 0 || strcmp(*argv, "--start") == 0)
        {
            if (argc--)
            {
                conf_first_picture = atoi(*(++argv));
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
        else if (strcmp(*argv, "-cx") == 0 || strcmp(*argv, "--camerastepx") == 0)
        {
            if (argc--)
            {
                conf_camera_step_x = atof(*(++argv));
            }
        }
        else if (strcmp(*argv, "-cy") == 0 || strcmp(*argv, "--camerastepy") == 0)
        {
            if (argc--)
            {
                conf_camera_step_y = atof(*(++argv));
            }
        }
        else if (strcmp(*argv, "-cz") == 0 || strcmp(*argv, "--camerastepz") == 0)
        {
            if (argc--)
            {
                conf_camera_step_z = atof(*(++argv));
            }
        }

        argv++;
    }

    printf("Initializing ...\n");
    Scenery* scenery = new Scenery();

    if (conf_file_path)
    {
        scenery->loadGlobal(conf_file_path);
    }
    else
    {
        scenery->autoPreset();
    }

    for (outputcount = 0; outputcount < conf_first_picture + conf_nb_pictures; outputcount++)
    {
        AtmosphereDefinition* atmo = scenery->getAtmosphere();
        atmo->hour = (int)floor(conf_daytime_start * 24.0);
        atmo->minute = (int)floor(fmod(conf_daytime_start, 1.0 / 24.0) * 24.0 * 60.0);
        atmo->validate();

        CameraDefinition* camera = scenery->getCamera();
        Vector3 step = {conf_camera_step_x, conf_camera_step_y, conf_camera_step_z};
        camera->setLocation(camera->getLocation().add(step));

        renderer = new SoftwareCanvasRenderer();
        renderer->setScenery(scenery);

        if (outputcount >= conf_first_picture)
        {
            sprintf(outputpath, "output/pic%05d.png", outputcount);
            startRender(renderer, outputpath, conf_render_params);
        }

        delete renderer;

        conf_daytime_start += conf_daytime_step;
    }

    printf("Cleaning up ...\n");
    delete scenery;

    printf("\rDone.                         \n");

    return 0;
}
