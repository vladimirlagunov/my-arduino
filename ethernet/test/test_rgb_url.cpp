#include "../rgb_url.hpp"
#include "gtest/gtest.h"


TEST(RgbUrl, from_query) {
    RgbUrl obj;
    EXPECT_EQ(obj.red, false);
    EXPECT_EQ(obj.green, false);
    EXPECT_EQ(obj.blue, false);

    obj.from_query("g=1", 4);
    EXPECT_EQ(obj.red, false);
    EXPECT_EQ(obj.green, true);
    EXPECT_EQ(obj.blue, false);

    obj.from_query("r=1&b=1", 8);
    EXPECT_EQ(obj.red, true);
    EXPECT_EQ(obj.green, true);
    EXPECT_EQ(obj.blue, true);

    obj.from_query("r=0&b=0", 8);
    EXPECT_EQ(obj.red, false);
    EXPECT_EQ(obj.green, true);
    EXPECT_EQ(obj.blue, false);

    obj.from_query("r=1&g=0&b=1", 12);
    EXPECT_EQ(obj.red, true);
    EXPECT_EQ(obj.green, false);
    EXPECT_EQ(obj.blue, true);
}


TEST(RgbUrl, dump) {
    RgbUrl obj;
    char buf[16];

    obj.dump(buf);
    EXPECT_STREQ(buf, "r=0&g=0&b=0");

    obj.green = true;
    obj.blue = true;
    obj.dump(buf);
    EXPECT_STREQ(buf, "r=0&g=1&b=1");

    obj.red = true;
    obj.blue = false;
    obj.dump(buf);
    EXPECT_STREQ(buf, "r=1&g=1&b=0");
}
