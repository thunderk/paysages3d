#include "DefinitionNode.h"

#include "Logs.h"
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
    int children_count = (int)children.size();
    stream->write(&children_count);

    for (auto child: children)
    {
        stream->write(child->name);

        int child_size = child->getStreamSize();
        if (child_size >= 0)
        {
            stream->write(&child_size);
            child->save(stream);
        }
        else
        {
            // Child size not known, write it to a temporary stream to know it
            Logs::debug() << "Unknown size for child " << child->name << ", unefficient writing to temporary stream" << std::endl;
            PackStream substream;
            child->save(&substream);
            stream->writeFromBuffer(substream, true);
        }
    }
}

void DefinitionNode::load(PackStream* stream)
{
    int children_count;

    stream->read(&children_count);

    for (int i = 0; i < children_count; i++)
    {
        std::string child_name = stream->readString();

        int child_size;
        stream->read(&child_size);

        DefinitionNode *child = findChildByName(child_name);
        if (child)
        {
            // TODO type check
            child->load(stream);
        }
        else
        {
            // TODO Ask subclass if it can instanciate a child
            // Else skip length of unknown child
            stream->skipBytes(child_size);
            Logs::warning() << "Skipped unknown child '" << child_name << "'" << std::endl;
        }
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
        Logs::warning() << "Trying to remove not found child '" << child->name << "' from '" << name << "'" << std::endl;
    }
}

DefinitionNode *DefinitionNode::findChildByName(const std::string name)
{
    for (auto child: children)
    {
        if (child->name == name)
        {
            return child;
        }
    }
    return NULL;
}

int DefinitionNode::getStreamSize() const
{
    return -1;
}
