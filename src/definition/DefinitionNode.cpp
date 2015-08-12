#include "DefinitionNode.h"

#include "PackStream.h"

DefinitionNode::DefinitionNode(DefinitionNode* parent, const std::string &name):
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

DefinitionNode::~DefinitionNode()
{
    if (parent)
    {
        parent->removeChild(this);
        parent = NULL;
    }

    // Work on a copy, because the child destructor will modify the array by removing itself using removeChild
    std::vector<DefinitionNode*> children_copy = children;
    for (auto child:children_copy)
    {
        if (child->getParent() == this)
        {
            delete child;
        }
    }
}

void DefinitionNode::setName(const std::string &name)
{
    this->name = name;
}

Scenery* DefinitionNode::getScenery()
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

std::string DefinitionNode::toString(int indent) const
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

void DefinitionNode::save(PackStream* stream) const
{
    stream->write(name);
    for (auto child: children)
    {
        child->save(stream);
    }
}

void DefinitionNode::load(PackStream* stream)
{
    name = stream->readString();
    for (auto child: children)
    {
        child->load(stream);
    }
}

void DefinitionNode::copy(DefinitionNode* destination) const
{
    destination->setName(name);
    // can't copy children as we don't know their types...
}

void DefinitionNode::validate()
{
    for (auto child: children)
    {
        child->validate();
    }
}

void DefinitionNode::addChild(DefinitionNode* child)
{
    if (std::find(children.begin(), children.end(), child) == children.end())
    {
        children.push_back(child);
        child->parent = this;
        child->root = this->root;
    }
}

void DefinitionNode::removeChild(DefinitionNode* child)
{
    std::vector<DefinitionNode*>::iterator it = std::find(children.begin(), children.end(), child);
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
