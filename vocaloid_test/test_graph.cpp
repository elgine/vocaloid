#include <stdio.h>
#include <audio_context.hpp>
using namespace vocaloid;

int main(){
    auto *ctx = new AudioContext();
    auto *node1 = new AudioNode(ctx),
        *node2 = new AudioNode(ctx);
    node1->name_ = "node1";
    node2->name_ = "node2";
    node1->Connect(node2);

    getchar();
    return 0;
}