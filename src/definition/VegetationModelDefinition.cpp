#include "VegetationModelDefinition.h"

#include "VegetationDefinition.h"
#include "RandomGenerator.h"
#include "Matrix4.h"
#include "SurfaceMaterial.h"
#include "Color.h"
#include "PackStream.h"

VegetationModelDefinition::VegetationModelDefinition(DefinitionNode *parent):
    DefinitionNode(parent, "model")
{
    solid_material = new SurfaceMaterial(Color(0.2, 0.15, 0.15));
    solid_material->reflection = 0.002;
    solid_material->shininess = 2.0;
    solid_material->hardness = 0.3;
    solid_material->validate();

    foliage_material = new SurfaceMaterial(Color(0.4, 0.8, 0.45));
    foliage_material->reflection = 0.007;
    foliage_material->shininess = 12.0;
    foliage_material->hardness = 0.2;
    foliage_material->validate();

    randomize();
}

VegetationModelDefinition::~VegetationModelDefinition()
{
    delete solid_material;
    delete foliage_material;
}

void VegetationModelDefinition::save(PackStream *stream) const
{
    int n;
    solid_material->save(stream);
    foliage_material->save(stream);

    n = solid_volumes.size();
    stream->write(&n);
    for (const auto &solid_volume: solid_volumes)
    {
        solid_volume.save(stream);
    }
    n = foliage_groups.size();
    stream->write(&n);
    for (const auto &foliage_group: foliage_groups)
    {
        foliage_group.save(stream);
    }
    n = foliage_items.size();
    stream->write(&n);
    for (const auto &foliage_item: foliage_items)
    {
        foliage_item.save(stream);
    }
}

void VegetationModelDefinition::load(PackStream *stream)
{
    int i, n;
    solid_material->load(stream);
    foliage_material->load(stream);

    stream->read(&n);
    solid_volumes.clear();
    for (i = 0; i < n; i++)
    {
        CappedCylinder solid_volume;
        solid_volume.load(stream);
        solid_volumes.push_back(solid_volume);
    }
    stream->read(&n);
    foliage_groups.clear();
    for (i = 0; i < n; i++)
    {
        Sphere foliage_group;
        foliage_group.load(stream);
        foliage_groups.push_back(foliage_group);
    }
    stream->read(&n);
    foliage_items.clear();
    for (i = 0; i < n; i++)
    {
        Disk foliage_item;
        foliage_item.load(stream);
        foliage_items.push_back(foliage_item);
    }
}

void VegetationModelDefinition::copy(DefinitionNode *destination_) const
{
    VegetationModelDefinition *destination = (VegetationModelDefinition *)destination_;

    solid_material->copy(destination->solid_material);
    foliage_material->copy(destination->foliage_material);

    destination->solid_volumes.clear();
    for (const auto &solid_volume: solid_volumes)
    {
        destination->solid_volumes.push_back(CappedCylinder(solid_volume));
    }
    destination->foliage_groups.clear();
    for (const auto &foliage_group: foliage_groups)
    {
        destination->foliage_groups.push_back(Sphere(foliage_group));
    }
    destination->foliage_items.clear();
    for (const auto &foliage_item: foliage_items)
    {
        destination->foliage_items.push_back(Disk(foliage_item));
    }
}

void VegetationModelDefinition::validate()
{
}

static inline double randomizeValue(double base, double min_factor, double max_factor)
{
    return base * (min_factor + RandomGenerator::random() * (max_factor - min_factor));
}

static void addBranchRecurse(std::vector<CappedCylinder> &branches, const Vector3 &base, const Vector3 &direction, double radius, double length)
{
    branches.push_back(CappedCylinder(base, direction, radius, length));

    if (length > 0.1)
    {
        int split_count = 3;
        Matrix4 pivot1 = Matrix4::newRotateAxis(randomizeValue(1.0 - 0.6 * length, 0.9, 1.1), VECTOR_EAST);
        Vector3 new_direction = pivot1.multPoint(direction);
        for (int i = 0; i < split_count; i++)
        {
            Matrix4 pivot2 = Matrix4::newRotateAxis(randomizeValue(M_PI * 2.0 / (double)split_count, 0.9, 1.1), direction);
            new_direction = pivot2.multPoint(new_direction);

            Vector3 new_base = base.add(direction.scale(randomizeValue(length, 0.4, 1.0)));
            if (new_base.add(new_direction).y > 0.1)
            {
                addBranchRecurse(branches, new_base, new_direction, randomizeValue(radius, 0.45, 0.6), randomizeValue(length, 0.55, 0.85));
            }
        }
    }
}

void VegetationModelDefinition::randomize()
{
    // Clear structure
    solid_volumes.clear();
    foliage_groups.clear();
    foliage_items.clear();

    // Add trunk and branches
    addBranchRecurse(solid_volumes, VECTOR_ZERO, VECTOR_UP, randomizeValue(0.05, 0.6, 1.0), randomizeValue(0.5, 0.8, 1.0));

    // Add foliage groups
    for (const auto &branch: solid_volumes)
    {
        double length = branch.getLength();
        if (length < 0.2)
        {
            double radius = length * 0.5;
            Vector3 center = branch.getAxis().getOrigin().add(branch.getAxis().getDirection().scale(radius));
            foliage_groups.push_back(Sphere(center, radius * 3.0));
        }
    }

    // Add foliage items
    for (int i = 0; i < 30; i++)
    {
        double radius = 0.15;
        double scale = randomizeValue(radius, 0.5, 1.0);
        Vector3 dir = Vector3::randomInSphere(1.0 - radius);
        Vector3 normal = dir.add(Vector3::randomInSphere(0.4)).add(Vector3(0.0, 0.3, 0.0)).normalize();
        Disk leaf(dir, normal, scale);
        foliage_items.push_back(leaf);
    }
}
