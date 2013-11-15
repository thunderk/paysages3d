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
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        delete it.next();
    }
}

void BaseDefinition::setName(QString name)
{
    this->name = name;
}

void BaseDefinition::save(PackStream* stream) const
{
    stream->write(name);
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->save(stream);
    }
}

void BaseDefinition::load(PackStream* stream)
{
    name = stream->readString();
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->load(stream);
    }
}

void BaseDefinition::copy(BaseDefinition* destination) const
{
    destination->setName(name);
    // can't copy children as we don't know their types...
}

void BaseDefinition::validate()
{
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->validate();
    }
}

void BaseDefinition::addChild(BaseDefinition* child)
{
    if (not children.contains(child))
    {
        children.append(child);
        child->parent = this;
        child->root = this->root;
    }
}

void BaseDefinition::removeChild(BaseDefinition* child)
{
    children.removeOne(child);
}
