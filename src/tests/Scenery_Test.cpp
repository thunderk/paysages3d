#include "BaseTestCase.h"

#include "System.h"
#include "Scenery.h"
#include "Logs.h"
#include "TerrainDefinition.h"
#include "AtmosphereDefinition.h"
#include "FloatNode.h"

TEST(Scenery, saveGlobal) {
    Scenery scenery1;
    scenery1.autoPreset();
    scenery1.getTerrain()->propWaterHeight()->setValue(0.2);
    scenery1.getAtmosphere()->setDayTime(0.53);

    Scenery::FileOperationResult result = scenery1.saveGlobal(TMP_DIRECTORY "test_paysages_scenery");
    EXPECT_EQ(Scenery::FILE_OPERATION_OK, result);

    Scenery scenery2;
    result = scenery2.loadGlobal(TMP_DIRECTORY "test_paysages_scenery");
    EXPECT_EQ(Scenery::FILE_OPERATION_OK, result);

    EXPECT_DOUBLE_EQ(0.2, scenery2.getTerrain()->propWaterHeight()->getValue());
    EXPECT_DOUBLE_EQ(0.53, scenery2.getAtmosphere()->getDaytime());
}
