//#include <stdio.h>
//#include <random>
//#include "../processor/audio_context.hpp"
//#include "../processor/audio_gain_node.hpp"
//#include <gtest/gtest.h>
//using namespace vocaloid;
//
//struct GainValueInput{
//    float value;
//    uint64_t timestamp;
//};
//
//TEST(TestGainNode, Normal){
//    auto *ctx = new AudioContext();
//    auto *gain = new AudioGainNode(ctx);
//    gain->gain_->value = 2.0f;
//    gain->gain_->SetValueAtTime(1, 500);
//
//    vector<GainValueInput> input = {
//        {0.5, 200},
//        {0.8, 50},
//        {0.6, 100},
//        {0.75, 400}
//    };
//
//    default_random_engine defaultEngine;
//    std::shuffle(input.begin(), input.end(), defaultEngine);
//
//    for(auto i : input){
//        gain->gain_->LinearRampToValueAtTime(i.value, i.timestamp);
//    }
////    gain->gain_->LinearRampToValueAtTime(0.5, 200);
////    gain->gain_->LinearRampToValueAtTime(0.8, 50);
////    gain->gain_->LinearRampToValueAtTime(0.6, 100);
////    gain->gain_->LinearRampToValueAtTime(0.75, 400);
//
//    ASSERT_EQ(gain->gain_->GetValueAtTime(25), 2);
//    ASSERT_EQ(gain->gain_->GetValueAtTime(600), 1);
//    ASSERT_EQ(gain->gain_->GetValueAtTime(350), 0.5 + (0.75 - 0.5) * (350 - 200)/(400 - 200));
//}
//
//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}