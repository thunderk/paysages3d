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

BaseModelerTool::BaseModelerTool(MainModelerWindow *ui) : impl(new pimpl), ui(ui) {
}

BaseModelerTool::~BaseModelerTool() {
}

void BaseModelerTool::addIntBinding(const string &object, const string &property, const string &path, bool monitor) {
    auto node = static_cast<IntNode *>(ui->getScenery()->findByPath(path));
    if (node) {
        impl->int_bindings.push_back(make_unique<IntPropertyBind>(ui, object, property, node));

        if (monitor) {
            startWatching(ui->getScenery(), path, false);
        }
    } else {
        Logs::error("UI") << "Can't find int node for binding : " << path << endl;
    }
}

void BaseModelerTool::addFloatBinding(const string &object, const string &property, const string &path, bool monitor) {
    auto node = static_cast<FloatNode *>(ui->getScenery()->findByPath(path));
    if (node) {
        impl->float_bindings.push_back(make_unique<FloatPropertyBind>(ui, object, property, node));

        if (monitor) {
            startWatching(ui->getScenery(), path, false);
        }
    } else {
        Logs::error("UI") << "Can't find float node for binding : " << path << endl;
    }
}
