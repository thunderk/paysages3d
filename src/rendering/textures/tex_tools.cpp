#include "private.h"

#include <cmath>
#include "NoiseGenerator.h"

/*
 * Get the base presence factor of a layer, not accounting for other layers.
 */
double texturesGetLayerBasePresence(TexturesLayerDefinition* layer, TerrainResult terrain)
{
    return zoneGetValue(layer->terrain_zone, terrain.location, terrain.normal);
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
    n = layersCount(textures->layers);
    for (i = 0; i < n; i++)
    {
        TexturesLayerDefinition* layer = (TexturesLayerDefinition*)layersGetLayer(textures->layers, i);

        if (layer->displacement_height > 0.0)
        {
            disp += layer->displacement_height;
        }
    }
    return disp;
}
