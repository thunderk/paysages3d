#include "BaseTestCase.h"

#include "RenderProgress.h"

TEST(RenderProgress, RecursiveSections) {
    RenderProgress progress(2);
    EXPECT_DOUBLE_EQ(0.0, progress.get());

    progress.enterSub(5);
    EXPECT_DOUBLE_EQ(0.0, progress.get());

    progress.add();
    EXPECT_DOUBLE_EQ(0.1, progress.get());

    progress.add(2);
    EXPECT_DOUBLE_EQ(0.3, progress.get());

    progress.enterSub(4);
    EXPECT_DOUBLE_EQ(0.3, progress.get());

    progress.add(1);
    EXPECT_DOUBLE_EQ(0.325, progress.get());

    progress.exitSub();
    EXPECT_DOUBLE_EQ(0.4, progress.get());

    progress.exitSub();
    EXPECT_DOUBLE_EQ(0.5, progress.get());

    progress.add(1);
    EXPECT_DOUBLE_EQ(1.0, progress.get());
}
