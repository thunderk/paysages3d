#ifndef VEGETATIONMODELDEFINITION_H
#define VEGETATIONMODELDEFINITION_H

#include "definition_global.h"

#include <vector>
#include "DefinitionNode.h"
#include "Sphere.h"
#include "CappedCylinder.h"
#include "Disk.h"

namespace paysages {
namespace definition {

/**
 * Model of vegetation.
 */
class DEFINITIONSHARED_EXPORT VegetationModelDefinition: public DefinitionNode
{
public:
    VegetationModelDefinition(DefinitionNode *parent);
    virtual ~VegetationModelDefinition();

    inline const std::vector<CappedCylinder> &getSolidVolumes() const {return solid_volumes;}
    inline const std::vector<Sphere> &getFoliageGroups() const {return foliage_groups;}
    inline const std::vector<Disk> &getFoliageItems() const {return foliage_items;}

    virtual void save(PackStream *stream) const override;
    virtual void load(PackStream *stream) override;
    virtual void copy(DefinitionNode *destination) const override;
    virtual void validate() override;

    /**
     * Randomize the model geometry.
     */
    void randomize();

private:
    SurfaceMaterial *solid_material;
    SurfaceMaterial *foliage_material;
    std::vector<CappedCylinder> solid_volumes;
    std::vector<Sphere> foliage_groups;
    std::vector<Disk> foliage_items;
};

}
}

#endif // VEGETATIONMODELDEFINITION_H
