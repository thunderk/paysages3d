#include "private.h"

#include <cmath>
#include "NoiseGenerator.h"
#include "TexturesDefinition.h"
#include "TextureLayerDefinition.h"
#include "Zone.h"

/*
 * Get the base presence factor of a layer, not accounting for other layers.
 */
double texturesGetLayerBasePresence(TextureLayerDefinition* layer, TerrainResult terrain)
{
    return layer->terrain_zone->getValue(terrain.location, terrain.normal);
}

/*
 * Get triplanar noise value, depending on the normal direction.
 */
double texturesGetTriplanarNoise(NoiseGenerator* noise, Vector3 location, Vector3 normal)
{
    double noiseXY = noise->get2DTotal(location.x, location.y);
    double noiseXZ = noise->get2DTotal(location.x, location.z);
    double noiseYZ = noise->get2DTotal(location.y, location.z);

    double mXY = fabs(normal.z);
    double mXZ = fabs(normal.y);
    double mYZ = fabs(normal.x);
    double total = mXY + mXZ + mYZ;
    mXY /= total;
    mXZ /= total;
    mYZ /= total;

    return noiseXY * mXY + noiseXZ * mXZ + noiseYZ * mYZ;
}

double texturesGetMaximalDisplacement(TexturesDefinition* textures)
{
    int i, n;
    double disp = 0.0;
    n = textures->count();
    for (i = 0; i < n; i++)
    {
        TextureLayerDefinition* layer = textures->getTextureLayer(i);

        if (layer->displacement_height > 0.0)
        {
            disp += layer->displacement_height;
        }
    }
    return disp;
}
