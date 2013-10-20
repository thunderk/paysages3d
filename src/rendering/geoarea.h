#ifndef _PAYSAGES_GEOAREA_H_
#define _PAYSAGES_GEOAREA_H_

/* Geographic area definition */

#include "rendering_global.h"
#include "tools/pack.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double location_x;
    double location_z;
    double size_x;
    double size_z;
} GeoArea;

RENDERINGSHARED_EXPORT GeoArea geoareaCreate();
RENDERINGSHARED_EXPORT void geoareaDelete(GeoArea* geoarea);
RENDERINGSHARED_EXPORT void geoareaCopy(GeoArea* source, GeoArea* destination);
RENDERINGSHARED_EXPORT void geoareaValidate(GeoArea* geoarea);

RENDERINGSHARED_EXPORT void geoareaSave(PackStream* stream, GeoArea* geoarea);
RENDERINGSHARED_EXPORT void geoareaLoad(PackStream* stream, GeoArea* geoarea);

RENDERINGSHARED_EXPORT void geoareaToLocal(GeoArea* geoarea, double absolute_x, double absolute_z, double* local_x, double* local_z);
RENDERINGSHARED_EXPORT void geoareaFromLocal(GeoArea* geoarea, double local_x, double local_z, double* absolute_x, double* absolute_z);

#ifdef __cplusplus
}
#endif

#endif
