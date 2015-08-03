#include "BaseTestCase.h"

#include "Scenery.h"

TEST(Scenery, saveGlobal)
{
    Scenery scenery;
    scenery.autoPreset();

    Scenery::FileOperationResult result = scenery.saveGlobal("/tmp/test_paysages_scenery");
    EXPECT_EQ((int)Scenery::FILE_OPERATION_OK, result);

    result = scenery.loadGlobal("/tmp/test_paysages_scenery");
    EXPECT_EQ((int)Scenery::FILE_OPERATION_OK, result);
}
