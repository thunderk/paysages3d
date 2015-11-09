#ifndef MEMORY_H
#define MEMORY_H

#include "system_global.h"

namespace paysages {
namespace system {

class SYSTEMSHARED_EXPORT Memory {
  public:
    static void *naiveArrayInsert(void **array, unsigned long item_size, int item_count, int location);
};
}
}

#endif // MEMORY_H
