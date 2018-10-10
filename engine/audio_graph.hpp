#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include "disposable.h"
using namespace std;
namespace vocaloid{

    class AudioGraph: public IDisposable{

    public:
        void AddNode(AudioNode* node) {

        }

        void RemoveNode(AudioNode* node) {

        }

        void Connect(AudioNode *from, AudioNode *to) {

        }

        void Disconnect(AudioNode *from, AudioNode *to) {

        }

        void Dispose() override {

        }
    };
}