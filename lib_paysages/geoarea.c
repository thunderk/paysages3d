#include "geoarea.h"

#include "tools.h"

GeoArea geoareaCreate()
{
    GeoArea result;
    
    result.location_x = -40.0;
    result.location_z = -40.0;
    result.size_x = 80.0;
    result.size_z = 80.0;
    
    return result;
}

void geoareaDelete(GeoArea* geoarea)
{
    UNUSED(geoarea);
}

void geoareaCopy(GeoArea* source, GeoArea* destination)
{
    *destination = *source;
}

void geoareaValidate(GeoArea* geoarea)
{
    if (geoarea->size_x < 0.000000001)
    {
        geoarea->size_x = 0.000000001;
    }
    if (geoarea->size_z < 0.000000001)
    {
        geoarea->size_z = 0.000000001;
    }
}

void geoareaSave(PackStream* stream, GeoArea* geoarea)
{
    packWriteDouble(stream, &geoarea->location_x);
    packWriteDouble(stream, &geoarea->location_z);
    packWriteDouble(stream, &geoarea->size_x);
    packWriteDouble(stream, &geoarea->size_z);
}

void geoareaLoad(PackStream* stream, GeoArea* geoarea)
{
    packReadDouble(stream, &geoarea->location_x);
    packReadDouble(stream, &geoarea->location_z);
    packReadDouble(stream, &geoarea->size_x);
    packReadDouble(stream, &geoarea->size_z);
}

void geoareaToLocal(GeoArea* geoarea, double absolute_x, double absolute_z, double* local_x, double* local_z)
{
    *local_x = (absolute_x - geoarea->location_x) / geoarea->size_x;
    *local_z = (absolute_z - geoarea->location_z) / geoarea->size_z;
}

void geoareaFromLocal(GeoArea* geoarea, double local_x, double local_z, double* absolute_x, double* absolute_z)
{
    *absolute_x = geoarea->location_x + local_x * geoarea->size_x;
    *absolute_z = geoarea->location_z + local_z * geoarea->size_z;
}
