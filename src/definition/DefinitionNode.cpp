#include "DefinitionNode.h"

#include "DefinitionDiff.h"
#include "DefinitionWatcher.h"
#include "DiffManager.h"
#include "Logs.h"
#include "PackStream.h"

#include <algorithm>
#include <cassert>

// Diff for abstract nodes
class DefinitionNodeDiff : public DefinitionDiff {
  public:
    DefinitionNodeDiff(const DefinitionNode *node) : DefinitionDiff(node) {
    }

    DefinitionNodeDiff(const DefinitionDiff *diff) : DefinitionDiff(diff) {
    }

    virtual DefinitionDiff *newReversed() const override {
        return new DefinitionNodeDiff(this);
    }
};

DefinitionNode::DefinitionNode(DefinitionNode *parent, const string &name, const string &type_name)
    : parent(parent), type_name(type_name), name(name) {
    if (parent) {
        root = parent->root;
        diffs = NULL;
        parent->addChild(this);
    } else {
        root = this;
        diffs = new DiffManager(this);
    }
}

DefinitionNode::~DefinitionNode() {
    if (parent) {
        parent->removeChild(this);
        parent = NULL;
    }

    if (diffs) {
        delete diffs;
        diffs = NULL;
    }

    // Work on a copy, because the child destructor will modify the array by removing itself using removeChild
    vector<DefinitionNode *> children_copy = children;
    for (auto child : children_copy) {
        if (child->getParent() == this) {
            delete child;
        }
    }
}

void DefinitionNode::setName(const string &name) {
    this->name = name;
}

const Scenery *DefinitionNode::getScenery() const {
    if (parent) {
        return parent->getScenery();
    } else {
        return NULL;
    }
}

string DefinitionNode::toString(int indent) const {
    string result;
    for (int i = 0; i < indent; i++) {
        result += " ";
    }
    result += name;
    if (not children.empty()) {
        for (auto &child : children) {
            result += "\n" + child->toString(indent + 1);
        }
    }
    return result;
}

string DefinitionNode::getPath() const {
    if (parent == root) {
        return parent->getPath() + name;
    } else if (parent) {
        return parent->getPath() + "/" + name;
    } else {
        return "/";
    }
}

DefinitionNode *DefinitionNode::findByPath(const string &path) const {
    if (path.empty()) {
        return NULL;
    } else if (path[0] == '/') {
        if (path.length() == 1) {
            return root;
        } else if (root == this) {
            return findByPath(path.substr(1));
        } else {
            return root->findByPath(path);
        }
    } else {
        size_t seppos = path.find("/");
        string child_name = (seppos == string::npos) ? path : path.substr(0, seppos);
        DefinitionNode *child = findChildByName(child_name);
        if (child) {
            if (seppos == string::npos) {
                return child;
            } else {
                return child->findByPath(path.substr(seppos + 1));
            }
        } else {
            return NULL;
        }
    }
}

bool DefinitionNode::applyDiff(const DefinitionDiff *diff, bool) {
    // Only do type check, subclasses will do the rest
    if (diff->getTypeName() == type_name) {
        return true;
    } else {
        Logs::error("Definition") << "Can't apply " << diff->getTypeName() << " diff to " << getName() << " "
                                  << type_name << " node" << endl;
        return false;
    }
}

void DefinitionNode::generateInitDiffs(vector<const DefinitionDiff *> *diffs) const {
    diffs->push_back(new DefinitionNodeDiff(this));
    // TODO add children diffs in cascade ?
}

void DefinitionNode::save(PackStream *stream) const {
    int children_count = static_cast<int>(children.size());
    stream->write(&children_count);

    for (auto child : children) {
        stream->write(child->name);

        int child_size = child->getStreamSize();
        if (child_size >= 0) {
            stream->write(&child_size);
            child->save(stream);
        } else {
            // Child size not known, write it to a temporary stream to know it
            Logs::debug("Definition") << "Unknown size for child " << child->name
                                      << ", unefficient writing to temporary stream" << endl;
            PackStream substream;
            child->save(&substream);
            stream->writeFromBuffer(substream, true);
        }
    }
}

void DefinitionNode::load(PackStream *stream) {
    int children_count;

    stream->read(&children_count);

    for (int i = 0; i < children_count; i++) {
        string child_name = stream->readString();

        int child_size;
        stream->read(&child_size);

        DefinitionNode *child = findChildByName(child_name);
        if (child) {
            // TODO type check
            child->load(stream);
        } else {
            // TODO Ask subclass if it can instanciate a child
            // Else skip length of unknown child
            stream->skipBytes(child_size);
            Logs::warning("Definition") << "Skipped unknown child '" << child_name << "'" << endl;
        }
    }
}

void DefinitionNode::copy(DefinitionNode *destination) const {
    if (destination->getTypeName() == getTypeName()) {
        destination->setName(name);
        for (auto &child : children) {
            DefinitionNode *dest_child = destination->findChildByName(child->name);
            if (dest_child) {
                child->copy(dest_child);
            } else {
                Logs::warning("Definition") << "Can't copy to child " << child->name << " of "
                                            << destination->getTypeName() << endl;
            }
        }
    } else {
        Logs::error("Definition") << "Can't copy from " << getTypeName() << " to " << destination->getTypeName()
                                  << endl;
    }
}

void DefinitionNode::validate() {
    // TODO This should be deprecated in favor of onChildChange
    for (auto child : children) {
        child->validate();
    }
}

void DefinitionNode::addChild(DefinitionNode *child) {
    if (find(children.begin(), children.end(), child) == children.end()) {
        if (child->parent != this) {
            child->parent = this;
            child->setRoot(root);
        }
        children.push_back(child);
    }
}

void DefinitionNode::removeChild(DefinitionNode *child) {
    vector<DefinitionNode *>::iterator it = find(children.begin(), children.end(), child);
    if (it != children.end()) {
        child->parent = NULL;
        children.erase(it);
    } else {
        Logs::warning("Definition") << "Trying to remove not found child '" << child->name << "' from '" << name << "'"
                                    << endl;
    }
}

DefinitionNode *DefinitionNode::findChildByName(const string &name) const {
    for (auto child : children) {
        if (child->name == name) {
            return child;
        }
    }
    return NULL;
}

void DefinitionNode::tellChanged() {
    if (parent) {
        parent->onChildChanged(0, name);
    }
}

void DefinitionNode::onChildChanged(int depth, const string &relpath) {
    if (parent) {
        parent->onChildChanged(depth + 1, name + "/" + relpath);
    }
}

int DefinitionNode::getStreamSize() const {
    return -1;
}

void DefinitionNode::addDiff(const DefinitionDiff *diff) {
    assert(diff->getTypeName() == type_name);

    if (root && root->diffs) {
        root->diffs->addDiff(this, diff);
    } else {
        // No diff manager available, apply it directly and delete it
        applyDiff(diff);
        delete diff;
    }
}

void DefinitionNode::setRoot(DefinitionNode *root) {
    this->root = root;
    if (diffs) {
        delete diffs;
        diffs = NULL;
    }
    for (auto &child : children) {
        child->setRoot(root);
    }
}
