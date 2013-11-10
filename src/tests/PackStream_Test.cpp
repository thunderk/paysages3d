#include "PackStream_Test.h"
CPPUNIT_TEST_SUITE_REGISTRATION(PackStream_Test);

#include "PackStream.h"

void PackStream_Test::testPack()
{
    PackStream* stream;
    int i;
    int data_i;
    double data_d;
    char* data_s;
    char buffer[100];

    /* Writing to pack */
    stream = packWriteFile("/tmp/test_paysages_pack");
    for (i = 0; i < 5; i++)
    {
        data_i = i;
        packWriteInt(stream, &data_i);

        data_d = (double)i;
        packWriteDouble(stream, &data_d);

        data_s = "Testing string 0123 (accentué) !";
        packWriteString(stream, data_s, 100);
        packWriteString(stream, data_s, 4);
        packWriteString(stream, data_s, 5);
    }
    packCloseStream(stream);

    /* Reading from pack */
    stream = packReadFile("/tmp/test_paysages_pack");
    for (i = 0; i < 5; i++)
    {
        packReadInt(stream, &data_i);
        ck_assert_int_eq(data_i, i);

        packReadDouble(stream, &data_d);
        ck_assert_double_eq(data_d, (double)i);

        packReadString(stream, buffer, 100);
        ck_assert_str_eq(buffer, "Testing string 0123 (accentué) !");
        packReadString(stream, buffer, 4);
        ck_assert_str_eq(buffer, "Tes");
        packReadString(stream, buffer, 3);
        ck_assert_str_eq(buffer, "Te");
    }
    packCloseStream(stream);
}
