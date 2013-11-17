#ifndef TERRAINHEIGHTMAP_H
#define TERRAINHEIGHTMAP_H

#include "definition_global.h"

#include "BaseDefinition.h"

namespace paysages {
namespace definition {

class TerrainHeightMapData;

class DEFINITIONSHARED_EXPORT TerrainHeightMap : public BaseDefinition
{
public:
    TerrainHeightMap(TerrainDefinition* terrain);
    virtual ~TerrainHeightMap();

    virtual void copy(BaseDefinition* destination) const override;
    virtual void save(PackStream* stream) const override;
    virtual void load(PackStream* stream) override;

    inline TerrainDefinition* getTerrain() const {return terrain;}
    int getInterpolatedHeight(double x, double z, double* result);
    int getGridHeight(int x, int z, double* result);

    bool isPainted(int x, int z);
    unsigned long getMemoryStats() const;

    void clearPainting();
    void brushElevation(const TerrainHeightMapBrush &brush, double value);
    void brushSmooth(const TerrainHeightMapBrush &brush, double value);
    void brushAddNoise(const TerrainHeightMapBrush &brush, NoiseGenerator* generator, double value);
    void brushReset(const TerrainHeightMapBrush &brush, double value);
    void brushFlatten(const TerrainHeightMapBrush &brush, double height, double force);
    void endBrushStroke();

    friend class TerrainHeightMapBrush;
    friend class TerrainHeightMapBrushSmooth;
    friend class TerrainHeightMapBrushReset;

protected:
    double* getDataPointer(TerrainHeightMapData* data, int x, int z, TerrainHeightMapData* fallback, TerrainDefinition* terrain, int grow);

private:
    TerrainDefinition* terrain;
    TerrainHeightMapData* merged_data;
    TerrainHeightMapData* brush_data;
};

}
}

#endif // TERRAINHEIGHTMAP_H
