#include "FloatNode.h"

#include "PackStream.h"
#include "FloatDiff.h"
#include "Logs.h"
#include <sstream>
#include <cassert>

FloatNode::FloatNode(DefinitionNode *parent, const string &name, double value)
    : DefinitionNode(parent, name, "float"), value(value) {
}

string FloatNode::toString(int indent) const {
    ostringstream stream;

    stream << DefinitionNode::toString(indent) << " " << value;

    return stream.str();
}

void FloatNode::save(PackStream *stream) const {
    stream->write(&value);
}

void FloatNode::load(PackStream *stream) {
    stream->read(&value);
}

void FloatNode::copy(DefinitionNode *destination) const {
    if (auto tdest = dynamic_cast<FloatNode *>(destination)) {
        tdest->value = value;
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

void FloatNode::setValue(double new_value) {
    addDiff(produceDiff(new_value));
}

const FloatDiff *FloatNode::produceDiff(double new_value) const {
    return new FloatDiff(this, value, new_value);
}

void FloatNode::generateInitDiffs(vector<const DefinitionDiff *> *diffs) const {
    diffs->push_back(produceDiff(value));
}

bool FloatNode::applyDiff(const DefinitionDiff *diff, bool backward) {
    if (!DefinitionNode::applyDiff(diff, backward)) {
        return false;
    }

    assert(diff->getTypeName() == "float");
    auto float_diff = dynamic_cast<const FloatDiff *>(diff);

    if (float_diff) {
        double previous = backward ? float_diff->getNewValue() : float_diff->getOldValue();
        double next = backward ? float_diff->getOldValue() : float_diff->getNewValue();

        if (value == previous) {
            value = next;
            tellChanged();
            return true;
        } else {
            Logs::error("Definition") << "Can't apply float diff " << previous << " => " << next << " to " << getName()
                                      << endl;
            return false;
        }
    } else {
        Logs::error("Could not cast DefinitionDiff to IntDiff");
        return false;
    }
}

void FloatNode::addValue(double added) {
    setValue(value + added);
}
