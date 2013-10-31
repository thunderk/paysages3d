#ifndef BASEDEFINITION_H
#define BASEDEFINITION_H

#include "definition_global.h"

#include <QList>
#include <QString>
#include "PackStream.h" // TODO Delete when c++ migration is done

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

    virtual void save(PackStream* pack);
    virtual void load(PackStream* pack);

    virtual void copy(BaseDefinition* destination);
    virtual void validate();

    inline const QString& getName() {return name;}
    virtual void setName(QString name);

    inline const BaseDefinition* getParent() {return parent;}
    inline const BaseDefinition* getRoot() {return root;}

protected:
    void addChild(BaseDefinition* child);
    void removeChild(BaseDefinition* child);

private:
    BaseDefinition* parent;
    BaseDefinition* root;
    QString name;
    QList<BaseDefinition*> children;
};

}
}

#endif // BASEDEFINITION_H
