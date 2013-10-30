#ifndef BASEDEFINITION_H
#define BASEDEFINITION_H

#include "definition_global.h"
#include <QVector>

class PackStream;

namespace paysages {
namespace definition {

/**
 * @brief Base class for all definition containers
 */
class DEFINITIONSHARED_EXPORT BaseDefinition
{
public:
    BaseDefinition(BaseDefinition* parent);
    virtual ~BaseDefinition();

    void addChild(BaseDefinition* child);
    void removeChild(BaseDefinition* child);

    virtual void save(PackStream* pack);
    virtual void load(PackStream* pack);

private:
    BaseDefinition* parent;
    QVector<BaseDefinition*> children;
};

}
}

#endif // BASEDEFINITION_H
