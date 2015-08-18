#include "BaseTestCase.h"

#include "AtmosphereDefinition.h"

static void check_daytime(const AtmosphereDefinition &atmo, int expected_hour, int expected_minute=0, int expected_second=0)
{
    int hour, minute, second;
    atmo.getHMS(&hour, &minute, &second);
    EXPECT_EQ(expected_hour, hour);
    EXPECT_EQ(expected_minute, minute);
    EXPECT_EQ(expected_second, second);
}

TEST(AtmosphereDefinition, setDayTime)
{
    AtmosphereDefinition atmo(NULL);

    atmo.setDayTime(0.0);
    check_daytime(atmo, 0);

    atmo.setDayTime(0.1);
    check_daytime(atmo, 2, 24);

    atmo.setDayTime(0.25);
    check_daytime(atmo, 6);

    atmo.setDayTime(0.5);
    check_daytime(atmo, 12);

    atmo.setDayTime(1.0);
    check_daytime(atmo, 0);

    atmo.setDayTime(-0.5);
    check_daytime(atmo, 12);

    atmo.setDayTime(1.5);
    check_daytime(atmo, 12);
}
