#include "BaseModelerTool.h"

#include "FloatNode.h"
#include "FloatPropertyBind.h"
#include "IntNode.h"
#include "IntPropertyBind.h"
#include "Logs.h"
#include "MainModelerWindow.h"
#include "Scenery.h"

class BaseModelerTool::pimpl {
  public:
    vector<unique_ptr<IntPropertyBind>> int_bindings;
    vector<unique_ptr<FloatPropertyBind>> float_bindings;
};

BaseModelerTool::BaseModelerTool(MainModelerWindow *ui)
    : DefinitionWatcher("BaseModelerTool"), impl(new pimpl), ui(ui) {
}

BaseModelerTool::~BaseModelerTool() {
}

void BaseModelerTool::destroy() {
    for (auto &binding : impl->int_bindings) {
        binding->unregister();
    }
    for (auto &binding : impl->float_bindings) {
        binding->unregister();
    }
    unregister();
}

void BaseModelerTool::addIntBinding(const string &object, const string &property, const string &path, bool monitor) {
    if (auto node = dynamic_cast<IntNode *>(ui->getScenery()->findByPath(path))) {
        impl->int_bindings.push_back(make_unique<IntPropertyBind>(ui, object, property, node));

        if (monitor) {
            startWatchingPath(ui->getScenery(), path, false);
        }
    } else {
        Logs::error("UI") << "Can't find int node for binding : " << path << endl;
    }
}

void BaseModelerTool::addFloatBinding(const string &object, const string &property, const string &path, bool monitor) {
    if (auto node = dynamic_cast<FloatNode *>(ui->getScenery()->findByPath(path))) {
        impl->float_bindings.push_back(make_unique<FloatPropertyBind>(ui, object, property, node));

        if (monitor) {
            startWatchingPath(ui->getScenery(), path, false);
        }
    } else {
        Logs::error("UI") << "Can't find float node for binding : " << path << endl;
    }
}
