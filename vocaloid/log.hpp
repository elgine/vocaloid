#pragma once
#include <stdin.h>
#include <fstream>
using namespace std;
namespace vocaloid{

    class Log{
    private:
        ofstream out_;
    public:
        void StartLog(const char* output_path){
            out_.open(output_path, ios::out);
        }

        void Write(const char* content){
            out_ << content;
        }

        void FinishLog(){
            out_.close();
        }
    };
}