#ifndef LEGACYLAYER_H
#define LEGACYLAYER_H

#include <PackStream.h>

typedef void* (*LayerCallbackCreate)();
typedef void (*LayerCallbackDelete)(void* layer);
typedef void (*LayerCallbackCopy)(void* source, void* definition);
typedef void (*LayerCallbackValidate)(void* layer);
typedef void (*LayerCallbackSave)(PackStream* stream, void* layer);
typedef void (*LayerCallbackLoad)(PackStream* stream, void* layer);

typedef struct {
    LayerCallbackCreate callback_create;
    LayerCallbackDelete callback_delete;
    LayerCallbackCopy callback_copy;
    LayerCallbackValidate callback_validate;
    LayerCallbackSave callback_save;
    LayerCallbackLoad callback_load;
} LayerType;

#ifdef __cplusplus

#include <BaseDefinition.h>

/**
 * @brief Wrapper around the old LayerType, used by C code.
 */
class LegacyLayer:public BaseDefinition
{
public:
    LegacyLayer(BaseDefinition* parent, LayerType* type);
    virtual ~LegacyLayer();

    virtual void save(PackStream* pack);
    virtual void load(PackStream* pack);

    virtual void copy(BaseDefinition* destination);
    virtual void validate();

    virtual void setName(QString name);

    inline void* getLegacyDefinition() { return legacy; }
    inline char* getLegacyName() {return legacy_name;}

private:
    LayerType type;
    void* legacy;
    char legacy_name[100];
};

#endif

#endif // LEGACYLAYER_H
