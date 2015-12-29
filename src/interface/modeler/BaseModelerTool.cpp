#include "BaseModelerTool.h"

#include "MainModelerWindow.h"
#include "Scenery.h"
#include "IntNode.h"
#include "IntPropertyBind.h"
#include "FloatNode.h"
#include "FloatPropertyBind.h"
#include "Logs.h"

class BaseModelerTool::pimpl {
  public:
    vector<unique_ptr<IntPropertyBind>> int_bindings;
    vector<unique_ptr<FloatPropertyBind>> float_bindings;
};

BaseModelerTool::BaseModelerTool(MainModelerWindow *main) : impl(new pimpl), main(main) {
}

BaseModelerTool::~BaseModelerTool() {
}

void BaseModelerTool::addIntBinding(const string &object, const string &property, const string &path) {
    auto node = static_cast<IntNode *>(main->getScenery()->findByPath(path));
    if (node) {
        impl->int_bindings.push_back(make_unique<IntPropertyBind>(main, object, property, node));
    } else {
        Logs::error("UI") << "Can't find int node for binding : " << path << endl;
    }
}

void BaseModelerTool::addFloatBinding(const string &object, const string &property, const string &path) {
    auto node = static_cast<FloatNode *>(main->getScenery()->findByPath(path));
    if (node) {
        impl->float_bindings.push_back(make_unique<FloatPropertyBind>(main, object, property, node));
    } else {
        Logs::error("UI") << "Can't find float node for binding : " << path << endl;
    }
}
