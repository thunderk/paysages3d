#include "BaseTestCase.h"

#include "PackStream.h"

TEST(PackStream, All)
{
    PackStream* stream;
    int i;
    int data_i;
    double data_d;
    char* data_s;
    char buffer[100];

    /* Writing to pack */
    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack", true);
    for (i = 0; i < 5; i++)
    {
        data_i = i;
        stream->write(&data_i);

        data_d = (double)i;
        stream->write(&data_d);

        data_s = "Testing string 0123 (accentué) !";
        stream->write(data_s, 100);
        stream->write(data_s, 4);
        stream->write(data_s, 5);
    }
    delete stream;

    /* Reading from pack */
    stream = new PackStream();
    stream->bindToFile("/tmp/test_paysages_pack");
    for (i = 0; i < 5; i++)
    {
        stream->read(&data_i);
        ASSERT_EQ(i, data_i);

        stream->read(&data_d);
        ASSERT_DOUBLE_EQ((double)i, data_d);

        stream->read(buffer, 100);
        ASSERT_STREQ("Testing string 0123 (accentué) !", buffer);
        stream->read(buffer, 4);
        ASSERT_STREQ("Tes", buffer);
        stream->read(buffer, 3);
        ASSERT_STREQ("Te", buffer);
    }
    delete stream;
}
