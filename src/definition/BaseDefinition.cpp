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

void BaseDefinition::save(PackStream* pack)
{
    pack->write(name);
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->save(pack);
    }
}

void BaseDefinition::load(PackStream* pack)
{
    name = pack->readString();
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->load(pack);
    }
}

void BaseDefinition::copy(BaseDefinition* destination)
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
