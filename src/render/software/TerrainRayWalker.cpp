#include "TerrainRayWalker.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "TexturesDefinition.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"

TerrainRayWalker::TerrainRayWalker(SoftwareRenderer* renderer):
    renderer(renderer)
{
}

void TerrainRayWalker::update()
{
    TerrainDefinition* terrain = renderer->getScenery()->getTerrain();
    HeightInfo info = terrain->getHeightInfo();

    TexturesDefinition* textures = renderer->getScenery()->getTextures();
    double disp = textures->getMaximalDisplacement();

    ymin = info.min_height - disp;
    ymax = info.max_height + disp;

    ydispmin = -disp;
    ydispmax = disp;

    minstep = 0.01 * terrain->scaling / (double)renderer->render_quality;
    maxstep = 1.0 * terrain->scaling;
}

bool TerrainRayWalker::startWalking(Vector3 start, const Vector3 &direction, double escaping_factor, double max_length, TerrainHitResult &result)
{
    TerrainRenderer* terrain_renderer = renderer->getTerrainRenderer();
    TexturesRenderer* textures_renderer = renderer->getTexturesRenderer();
    TerrainRenderer::TerrainResult terrain_result;
    Vector3 end, displaced;
    bool hit;
    double diff;

    double step_length = minstep;
    double walked_length = 0.0;

    do
    {
        // Perform a step
        end = start.add(direction.scale(step_length));

        // Get the terrain info at end (without textures displacement)
        terrain_result = terrain_renderer->getResult(end.x, end.z, true, false);
        diff = end.y - terrain_result.location.y;

        // If we are very under the terrain, consider a hit
        if (diff < ydispmin)
        {
            hit = true;
        }

        // If we are close enough to the terrain, apply displacement
        else if (diff < ydispmax)
        {
            displaced = textures_renderer->displaceTerrain(terrain_result);
            diff = end.y - displaced.y;
            hit = (diff < 0.0);
        }

        if (hit)
        {
            // Refine the hit with dichotomy at high quality
            /*if (renderer->render_quality > 7)
            {
                end = refineHit(start, end, step_length);
            }*/

            // Find an escape
            /*if (escaping_factor != 0.0)
            {
                result.escape_length = findEscape(end, walked_length, escaping_factor, max_length);
            }*/

            result.hit_location = end;
        }
        else
        {
            // Prepare next step
            start = end;
            walked_length += step_length;

            step_length = diff / (double)renderer->render_quality;
            if (step_length < minstep)
            {
                step_length = minstep;
            }
            else if (step_length > maxstep)
            {
                step_length = maxstep;
            }
        }
    } while (not hit and start.y < ymax and walked_length < max_length);

    return hit;
}
