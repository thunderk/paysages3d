#include "BaseTestCase.h"

#include "Scenery.h"

TEST(Scenery, saveGlobal)
{
    Scenery scenery1;
    scenery1.autoPreset();

    Scenery::FileOperationResult result = scenery1.saveGlobal("/tmp/test_paysages_scenery");
    EXPECT_EQ((int)Scenery::FILE_OPERATION_OK, result);

    Scenery scenery2;
    result = scenery2.loadGlobal("/tmp/test_paysages_scenery");
    EXPECT_EQ((int)Scenery::FILE_OPERATION_OK, result);
}
