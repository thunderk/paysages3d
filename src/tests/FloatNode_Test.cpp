#include "BaseTestCase.h"

#include "FloatNode.h"
#include "PackStream.h"

TEST(FloatNode, toString)
{
    FloatNode test(NULL, "test", 2.1);

    EXPECT_EQ("test 2.1", test.toString(0));
    EXPECT_EQ("  test 2.1", test.toString(2));
}

TEST(FloatNode, saveLoadAndSkip)
{
    DefinitionNode root1(NULL, "root");
    FloatNode testa1(&root1, "testa", 1.5);
    FloatNode testb1(&root1, "testb", 4.3);

    PackStream stream1;
    root1.save(&stream1);

    DefinitionNode root2(NULL, "root");
    FloatNode testb2(&root2, "testb");

    PackStream stream2(&stream1);
    root2.load(&stream2);

    EXPECT_DOUBLE_EQ(4.3, testb2.getValue());
}
