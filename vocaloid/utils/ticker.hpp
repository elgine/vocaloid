#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

class Ticker{
private:
    uint16_t fps_;
    float sec_per_frame_;
    clock_t start_timestamp_;
    clock_t cur_timestamp_;
    clock_t last_timestamp_;
    clock_t timestamp_;
    clock_t delta_;
    bool running_;

    clock_t pause_timestamp_;
    clock_t paused_timestamp_;
public:

    explicit Ticker(uint16_t fps = 30):fps_(fps){
        start_timestamp_ = 0;
        cur_timestamp_ = 0;
        last_timestamp_ = 0;
        delta_ = 0;
        timestamp_ = 0;
        paused_timestamp_ = 0;
        pause_timestamp_ = 0;
        sec_per_frame_ = 1.0f/fps_;
        running_ = false;
    }

    clock_t GetCurTimestamp(){
        return cur_timestamp_;
    }

    clock_t GetDelta(){
        return delta_;
    }

    void Start(uint64_t offset = 0){
        timestamp_ = last_timestamp_ = start_timestamp_ = clock();
        cur_timestamp_ = 0;
        paused_timestamp_ = 0;
        delta_ = 0;
        running_ = true;
    }

    void Update(){
        last_timestamp_ = timestamp_;
        timestamp_ = clock();
        delta_ = timestamp_ - last_timestamp_;
        cur_timestamp_ = timestamp_ - start_timestamp_ - paused_timestamp_;
    }

    void Pause(){
        running_ = false;
        pause_timestamp_ = clock();
    }

    void Resume(){
        clock_t t = clock();
        paused_timestamp_ += t - pause_timestamp_;
        running_ = true;
    }

    bool IsRunning(){
        return running_;
    }
};
