#pragma once
#include <gtest/gtest.h>
#include <vocaloid/utils/path.hpp>

TEST(TestPathUtil, Normal){
    string path = "\\Desktop\\output.wav";
    ASSERT_EQ(GetExtension(path), ".wav");
    ASSERT_EQ(GetSubDirectory(path), "\\Desktop");
    ASSERT_EQ(GetFileName(path), "output.wav");
    ASSERT_EQ(CombinePath("C:\\Users\\Elgine", path), "C:\\Users\\Elgine\\Desktop\\output.wav");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}