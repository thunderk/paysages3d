#ifndef _PAYSAGES_GEOAREA_H_
#define _PAYSAGES_GEOAREA_H_

/* Geographic area definition */

#include "pack.h"

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

GeoArea geoareaCreate();
void geoareaDelete(GeoArea* geoarea);
void geoareaCopy(GeoArea* source, GeoArea* destination);
void geoareaValidate(GeoArea* geoarea);

void geoareaSave(PackStream* stream, GeoArea* geoarea);
void geoareaLoad(PackStream* stream, GeoArea* geoarea);

void geoareaToLocal(GeoArea* geoarea, double absolute_x, double absolute_z, double* local_x, double* local_z);
void geoareaFromLocal(GeoArea* geoarea, double local_x, double local_z, double* absolute_x, double* absolute_z);
    
#ifdef __cplusplus
}
#endif

#endif
