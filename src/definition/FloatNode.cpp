#include "FloatNode.h"

#include "PackStream.h"
#include <sstream>

FloatNode::FloatNode(DefinitionNode* parent, const std::string &name, double value):
    DefinitionNode(parent, name), value(value)
{
}

std::string FloatNode::toString(int indent) const
{
    std::ostringstream stream;

    stream << DefinitionNode::toString(indent) << " " << value;

    return stream.str();
}

void FloatNode::save(PackStream *stream) const
{
    stream->write(&value);
}

void FloatNode::load(PackStream *stream)
{
    stream->read(&value);
}

void FloatNode::copy(DefinitionNode *destination) const
{
    // TODO type check

    ((FloatNode *)destination)->value = value;
}
