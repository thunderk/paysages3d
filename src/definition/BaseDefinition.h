#ifndef BASEDEFINITION_H
#define BASEDEFINITION_H

#include "definition_global.h"

#include <QList>
#include <QString>

namespace paysages {
namespace system {class PackStream;}
namespace definition {

/**
 * @brief Base class for all definition containers
 */
class DEFINITIONSHARED_EXPORT BaseDefinition
{
public:
    BaseDefinition(BaseDefinition* parent);
    virtual ~BaseDefinition();

    virtual void save(PackStream* stream) const;
    virtual void load(PackStream* stream);

    virtual void copy(BaseDefinition* destination) const;
    virtual void validate();

    inline const QString& getName() const {return name;}
    virtual void setName(QString name);

    inline const BaseDefinition* getParent() const {return parent;}
    inline const BaseDefinition* getRoot() const {return root;}

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
