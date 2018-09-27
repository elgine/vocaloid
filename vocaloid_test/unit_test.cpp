#include <iostream>
#include <stdio.h>
#include <string>
#include <gtest/gtest.h>
#include <tone.hpp>
#include <path.hpp>
using namespace std;
using namespace vocaloid;

TEST(TestFequencyToNote, Normal){
    string note;
    note = FreqToNote(27.500);
    ASSERT_STREQ("A0", note.c_str());
    note = FreqToNote(440.f);
    ASSERT_STREQ("A4", note.c_str());
    note = FreqToNote(466.164f);
    ASSERT_STREQ("A#4", note.c_str());
    note = FreqToNote(7902.133);
    ASSERT_STREQ("B8", note.c_str());
}

TEST(TestNoteToFequency, Normal){
    float frequency;
    frequency = NoteToFreq("A0");
    ASSERT_FLOAT_EQ(frequency, 27.500);
    frequency = NoteToFreq("A4");
    ASSERT_FLOAT_EQ(frequency, 440.f);
    frequency = NoteToFreq("A#4");
    ASSERT_FLOAT_EQ(frequency, 466.164);
    frequency = NoteToFreq("B8");
    ASSERT_FLOAT_EQ(frequency, 7902.133);
}

TEST(TestPathUtil, Normal){
    string path = "\\Desktop\\output.wav";
    ASSERT_EQ(GetExtension(path), ".wav");
    ASSERT_EQ(GetSubDirectory(path), "\\Desktop");
    ASSERT_EQ(GetFileName(path), "output.wav");
    ASSERT_EQ(CombinePath("C:\\Users\\Elgine", path), "C:\\Users\\Elgine\\Desktop\\output.wav");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}