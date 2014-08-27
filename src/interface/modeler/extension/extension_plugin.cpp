#include "extension_plugin.h"
#include "paysages.h"

#include <qqml.h>

void ExtensionPlugin::registerTypes(const char *uri)
{
    // @uri Paysages
    qmlRegisterType<Paysages>(uri, 1, 0, "Paysages");
}


