#include "BaseDefinition.h"

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

void BaseDefinition::addChild(BaseDefinition* child)
{
    if (not children.contains(child))
    {
        children.append(child);
    }
}

void BaseDefinition::removeChild(BaseDefinition* child)
{
    children.removeOne(child);
}

void BaseDefinition::save(PackStream* pack)
{
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->save(pack);
    }
}

void BaseDefinition::load(PackStream* pack)
{
    QListIterator<BaseDefinition*> it(children);
    while (it.hasNext())
    {
        it.next()->load(pack);
    }
}
