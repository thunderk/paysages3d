#include "MaterialNode.h"

#include "ColorNode.h"
#include "FloatNode.h"
#include "FractalNoise.h"
#include "NoiseNode.h"
#include "SurfaceMaterial.h"

MaterialNode::MaterialNode(DefinitionNode *parent, const string &name) : DefinitionNode(parent, name, "material") {
    material = make_unique<SurfaceMaterial>();

    diffuse = new ColorNode(this, "diffuse");
    ambient = new FloatNode(this, "ambient");
    hardness = new FloatNode(this, "hardness");
    reflection = new FloatNode(this, "reflection");
    shininess = new FloatNode(this, "shininess");
    bump = new NoiseNode(this, "bump");
}

const SurfaceMaterial &MaterialNode::getSurfaceMaterial() const {
    return *material;
}

void MaterialNode::setSurfaceMaterial(const SurfaceMaterial &material) {
    diffuse->setValue(*material.base);
    ambient->setValue(material.ambient);
    hardness->setValue(material.hardness);
    reflection->setValue(material.reflection);
    shininess->setValue(material.shininess);
}

void MaterialNode::setSurfaceMaterial(const Color &color, double reflection, double shininess, double hardness,
                                      double ambient) {
    this->diffuse->setValue(color);
    this->reflection->setValue(reflection);
    this->shininess->setValue(shininess);
    this->hardness->setValue(hardness);
    this->ambient->setValue(ambient);
}

bool MaterialNode::hasBump() const {
    return bump->getGenerator()->getHeight() > 0.0;
}

const FractalNoise *MaterialNode::getBumpGenerator() const {
    return bump->getGenerator();
}

void MaterialNode::onChildChanged(int depth, const string &relpath) {
    updateMaterial();

    DefinitionNode::onChildChanged(depth, relpath);
}

void MaterialNode::updateMaterial() {
    *material->base = diffuse->getValue();
    material->ambient = ambient->getValue();
    material->hardness = hardness->getValue();
    material->reflection = reflection->getValue();
    material->shininess = shininess->getValue();
}
