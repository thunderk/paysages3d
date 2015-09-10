#include "TerrainRayWalker.h"

#include "SoftwareRenderer.h"
#include "Scenery.h"
#include "TerrainDefinition.h"
#include "TexturesDefinition.h"
#include "TerrainRenderer.h"
#include "TexturesRenderer.h"
#include "Matrix4.h"

TerrainRayWalker::TerrainRayWalker(SoftwareRenderer* renderer):
    renderer(renderer)
{
    setQuality(0.5);
}

void TerrainRayWalker::setQuality(double displacement_safety, double minimal_step, double maximal_step, double step_factor, double max_distance, double escape_step)
{
    this->displacement_safety = displacement_safety;
    this->minimal_step = minimal_step;
    this->maximal_step = maximal_step;
    this->step_factor = step_factor;
    this->max_distance = max_distance;
    this->escape_step = escape_step;
}

void TerrainRayWalker::setQuality(double factor)
{
    setQuality(0.2 + 0.8 * factor,
               1.0 / (factor * factor * 30.0 + 1.0),
               50.0 / (factor * 10.0 + 1.0),
               1.0 / (factor * 10.0 + 1.0),
               10.0 + factor * 200.0,
               factor * factor * 100.0);
}

void TerrainRayWalker::update()
{
    TerrainDefinition* terrain = renderer->getScenery()->getTerrain();
    HeightInfo info = terrain->getHeightInfo();

    TexturesDefinition* textures = renderer->getScenery()->getTextures();
    displacement_base = textures->getMaximalDisplacement();

    ymin = info.min_height - displacement_base;
    ymax = info.max_height + displacement_base;
}

static inline Vector3 _getShiftAxis(const Vector3 &direction)
{
    if (fabs(direction.y) > 0.99)
    {
        // When the ray is vertical, we choose an arbitrary shift axis
        return VECTOR_NORTH;
    }
    else
    {
        return VECTOR_UP.crossProduct(direction);
    }
}

bool TerrainRayWalker::startWalking(const Vector3 &start, Vector3 direction, double escape_angle, TerrainHitResult &result)
{
    TerrainRenderer* terrain_renderer = renderer->getTerrainRenderer();
    TexturesRenderer* textures_renderer = renderer->getTexturesRenderer();
    TerrainRenderer::TerrainResult terrain_result;
    Vector3 cursor, displaced;
    double diff;
    Matrix4 shift_matrix;
    double shift_step = 0.0;

    Vector3 previous_cursor = start;
    bool hit = false;
    double step_length = minimal_step;
    double walked_length = 0.0;

    result.escape_angle = 0.0;
    if (escape_angle != 0.0)
    {
        // Prepare escape
        shift_step = escape_angle / escape_step;
        shift_matrix = Matrix4::newRotateAxis(-shift_step, _getShiftAxis(direction));
    }

    do
    {
        // Perform a step
        cursor = previous_cursor.add(direction.scale(step_length));

        // Get the terrain info at end (without textures displacement)
        terrain_result = terrain_renderer->getResult(cursor.x, cursor.z, true, false);
        diff = cursor.y - terrain_result.location.y;

        // If we are very under the terrain, consider a hit
        if (diff < -displacement_base * displacement_safety)
        {
            hit = true;
        }

        // If we are close enough to the terrain, apply displacement
        else if (diff < displacement_base * displacement_safety)
        {
            displaced = textures_renderer->displaceTerrain(terrain_result);
            diff = cursor.y - displaced.y;
            hit = diff < 0.0;
        }

        if (hit)
        {
            // TODO Refine the hit with dichotomy at high quality
            /*if (renderer->render_quality > 7)
            {
                cursor = refineHit(previous_cursor, cursor, step_length);
            }*/

            // Shift ray to escape terrain
            if (escape_angle != 0.0)
            {
                result.escape_angle += shift_step;
                if (result.escape_angle > escape_angle)
                {
                    // Too much shifted to escape, make it a hit
                    result.escape_angle = 0.0;
                    return true;
                }
                hit = false;
                direction = shift_matrix.multPoint(direction);
                previous_cursor = start.add(shift_matrix.multPoint(previous_cursor.sub(start)));
            }

            result.hit_location = cursor;
        }
        else
        {
            // Prepare next step
            previous_cursor = cursor;
            walked_length += step_length;

            step_length = diff * step_factor;
            if (step_length < minimal_step)
            {
                step_length = minimal_step;
            }
            else if (step_length > maximal_step)
            {
                step_length = maximal_step;
            }
        }
    } while (not hit and cursor.y < ymax and walked_length < max_distance);

    return hit or result.escape_angle > 0.0;
}
