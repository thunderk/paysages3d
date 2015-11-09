#ifndef TERRAINHEIGHTMAP_H
#define TERRAINHEIGHTMAP_H

#include "definition_global.h"

#include "PaintedGrid.h"

namespace paysages {
namespace definition {

class DEFINITIONSHARED_EXPORT TerrainHeightMap : public PaintedGrid {
  public:
    TerrainHeightMap(TerrainDefinition *terrain);

    virtual void copy(DefinitionNode *destination) const override;

    inline TerrainDefinition *getTerrain() const {
        return terrain;
    }

    virtual double getInitialValue(double x, double y) const override;

    void brushElevation(const PaintedGridBrush &brush, double x, double y, double value, bool commit = false);
    void brushSmooth(const PaintedGridBrush &brush, double x, double y, double value, bool commit = false);
    void brushAddNoise(const PaintedGridBrush &brush, double x, double y, NoiseGenerator *generator, double value,
                       bool commit = false);
    void brushReset(const PaintedGridBrush &brush, double x, double y, double value, bool commit = false);
    void brushFlatten(const PaintedGridBrush &brush, double x, double y, double height, double force,
                      bool commit = false);

    virtual void clearPainting() override;
    virtual void endBrushStroke() override;

  private:
    TerrainDefinition *terrain;
};
}
}

#endif // TERRAINHEIGHTMAP_H
