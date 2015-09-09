#include "Thread.h"
#include "SoftwareCanvasRenderer.h"

class RenderThread: public Thread
{
public:
    RenderThread(SoftwareCanvasRenderer *renderer, const char *outputpath):
        renderer(renderer), outputpath(outputpath)
    {
    }

    virtual void run() override
    {
        renderer->render();
    }

private:
    SoftwareCanvasRenderer *renderer;
    const char *outputpath;
};

void startRender(SoftwareCanvasRenderer *renderer, const char *outputpath)
{
    RenderThread thread(renderer, outputpath);

    printf("\rRendering %s ...                   \n", outputpath);
    thread.start();

    while (thread.isWorking())
    {
        Thread::timeSleepMs(200);

        printf("\rProgress : %0.1f%%                         ", renderer->getProgress() * 100.0);
        fflush(stdout);
    }
    thread.join();

    printf("\rSaving %s ...                      \n", outputpath);
    remove(outputpath);
    renderer->saveToDisk(outputpath);
}

