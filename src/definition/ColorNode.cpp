#include "ColorNode.h"

#include <cassert>
#include <sstream>
#include "ColorDiff.h"
#include "Logs.h"

// TODO Could be made into a template

ColorNode::ColorNode(DefinitionNode *parent, const string &name, const Color &value)
    : DefinitionNode(parent, name, "color"), value(value) {
}

string ColorNode::toString(int indent) const {
    ostringstream stream;

    stream << DefinitionNode::toString(indent) << " " << value.r << "," << value.g << "," << value.b << "," << value.a;

    return stream.str();
}

void ColorNode::save(PackStream *stream) const {
    value.save(stream);
}

void ColorNode::load(PackStream *stream) {
    value.load(stream);
}

void ColorNode::copy(DefinitionNode *destination) const {
    if (auto tdest = dynamic_cast<ColorNode *>(destination)) {
        tdest->value = value;
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

void ColorNode::setValue(const Color &value) {
    addDiff(produceDiff(value));
}

const ColorDiff *ColorNode::produceDiff(const Color &value) const {
    return new ColorDiff(this, this->value, value);
}

void ColorNode::generateInitDiffs(vector<const DefinitionDiff *> *diffs) const {
    diffs->push_back(produceDiff(value));
}

bool ColorNode::applyDiff(const DefinitionDiff *diff, bool backward) {
    if (!DefinitionNode::applyDiff(diff, backward)) {
        return false;
    }

    assert(diff->getTypeName() == "color");
    auto color_diff = dynamic_cast<const ColorDiff *>(diff);

    if (color_diff) {
        Color previous = backward ? color_diff->getNewValue() : color_diff->getOldValue();
        Color next = backward ? color_diff->getOldValue() : color_diff->getNewValue();

        if (value == previous) {
            value = next;
            return true;
        } else {
            Logs::error("Definition") << "Can't apply color diff" << endl;
            return false;
        }
    } else {
        Logs::error("Could not cast DefinitionDiff to ColorDiff");
        return false;
    }
}
