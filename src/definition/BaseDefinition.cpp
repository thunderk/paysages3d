#include "BaseDefinition.h"

#include "PackStream.h"

BaseDefinition::BaseDefinition(BaseDefinition* parent, const std::string &name):
    parent(parent), name(name)
{
    if (parent)
    {
        root = parent->root;
        parent->addChild(this);
    }
    else
    {
        root = this;
    }
}

BaseDefinition::~BaseDefinition()
{
    if (parent)
    {
        parent->removeChild(this);
        parent = NULL;
    }

    // Work on a copy, because the child destructor will modify the array by removing itself using removeChild
    std::vector<BaseDefinition*> children_copy = children;
    for (auto child:children_copy)
    {
        if (child->getParent() == this)
        {
            delete child;
        }
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

std::string BaseDefinition::toString(int indent) const
{
    std::string result;
    for (int i = 0; i < indent; i++)
    {
        result += " ";
    }
    result += name;
    if (not children.empty())
    {
        for (auto &child: children)
        {
            result += "\n" + child->toString(indent + 1);
        }
    }
    return result;
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
        child->parent = NULL;
        children.erase(it);
    }
    else
    {
        qWarning("Trying to remove not found child '%s' from '%s'", child->name.c_str(), name.c_str());
    }
}
