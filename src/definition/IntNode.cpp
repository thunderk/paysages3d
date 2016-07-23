#include "IntNode.h"

#include "IntDiff.h"
#include "Logs.h"
#include "PackStream.h"
#include <cassert>
#include <sstream>

IntNode::IntNode(DefinitionNode *parent, const string &name, int value)
    : DefinitionNode(parent, name, "int"), value(value) {
}

string IntNode::toString(int indent) const {
    ostringstream stream;

    stream << DefinitionNode::toString(indent) << " " << value;

    return stream.str();
}

void IntNode::save(PackStream *stream) const {
    stream->write(&value);
}

void IntNode::load(PackStream *stream) {
    int val;
    stream->read(&val);
    setValue(val);
}

void IntNode::copy(DefinitionNode *destination) const {
    if (auto tdest = dynamic_cast<IntNode *>(destination)) {
        tdest->setValue(value);
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

void IntNode::setValue(int new_value) {
    addDiff(produceDiff(new_value));
}

const IntDiff *IntNode::produceDiff(int new_value) const {
    return new IntDiff(this, value, new_value);
}

void IntNode::generateInitDiffs(vector<const DefinitionDiff *> *diffs) const {
    diffs->push_back(produceDiff(value));
}

bool IntNode::applyDiff(const DefinitionDiff *diff, bool backward) {
    if (!DefinitionNode::applyDiff(diff, backward)) {
        return false;
    }

    assert(diff->getTypeName() == "int");
    auto int_diff = dynamic_cast<const IntDiff *>(diff);

    if (int_diff) {
        int previous = backward ? int_diff->getNewValue() : int_diff->getOldValue();
        int next = backward ? int_diff->getOldValue() : int_diff->getNewValue();

        if (value == previous) {
            value = next;
            tellChanged();
            return true;
        } else {
            Logs::error("Definition") << "Can't apply int diff " << previous << " => " << next << " to " << getName()
                                      << endl;
            return false;
        }
    } else {
        Logs::error("Could not cast DefinitionDiff to IntDiff");
        return false;
    }
}
