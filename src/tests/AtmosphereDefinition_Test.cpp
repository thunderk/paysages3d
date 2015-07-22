#include "BaseTestCase.h"

#include "AtmosphereDefinition.h"

TEST(AtmosphereDefinition, setDaytime)
{
    AtmosphereDefinition atmo(NULL);

    atmo.setDaytime(0.0);
    EXPECT_EQ(atmo.hour, 0);
    EXPECT_EQ(atmo.minute, 0);

    atmo.setDaytime(0.1);
    EXPECT_EQ(atmo.hour, 2);
    EXPECT_EQ(atmo.minute, 24);

    atmo.setDaytime(0.25);
    EXPECT_EQ(atmo.hour, 6);
    EXPECT_EQ(atmo.minute, 0);

    atmo.setDaytime(0.5);
    EXPECT_EQ(atmo.hour, 12);
    EXPECT_EQ(atmo.minute, 0);

    atmo.setDaytime(1.0);
    EXPECT_EQ(atmo.hour, 0);
    EXPECT_EQ(atmo.minute, 0);

    atmo.setDaytime(-0.5);
    EXPECT_EQ(atmo.hour, 12);
    EXPECT_EQ(atmo.minute, 0);

    atmo.setDaytime(1.5);
    EXPECT_EQ(atmo.hour, 12);
    EXPECT_EQ(atmo.minute, 0);
}
