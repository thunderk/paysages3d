#include "BaseDefinition.h"

#include "PackStream.h"

BaseDefinition::BaseDefinition(BaseDefinition* parent):
    parent(parent)
{
    if (parent)
    {
        root = parent->root;
    }
    else
    {
        root = this;
    }
}

BaseDefinition::~BaseDefinition()
{
    for (auto child:children)
    {
        delete child;
    }
}

void BaseDefinition::setName(const std::string &name)
{
    this->name = name;
}

Scenery* BaseDefinition::getScenery()
{
    if (parent)
    {
        return parent->getScenery();
    }
    else
    {
        return NULL;
    }
}

void BaseDefinition::save(PackStream* stream) const
{
    stream->write(name);
    for (auto child: children)
    {
        child->save(stream);
    }
}

void BaseDefinition::load(PackStream* stream)
{
    name = stream->readString();
    for (auto child: children)
    {
        child->load(stream);
    }
}

void BaseDefinition::copy(BaseDefinition* destination) const
{
    destination->setName(name);
    // can't copy children as we don't know their types...
}

void BaseDefinition::validate()
{
    for (auto child: children)
    {
        child->validate();
    }
}

void BaseDefinition::addChild(BaseDefinition* child)
{
    if (std::find(children.begin(), children.end(), child) == children.end())
    {
        children.push_back(child);
        child->parent = this;
        child->root = this->root;
    }
}

void BaseDefinition::removeChild(BaseDefinition* child)
{
    std::vector<BaseDefinition*>::iterator it = std::find(children.begin(), children.end(), child);
    if (it != children.end())
    {
        children.erase(it);
    }
    else
    {
        qWarning("Trying to remove not found child '%s' from '%s'", child->name.c_str(), name.c_str());
    }
}
