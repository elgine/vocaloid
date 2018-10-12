#include <iostream>
#include <vocaloid/common/buffer.hpp>
using namespace vocaloid;
int main(){
    Buffer<float> *buffer = new Buffer<float>(20);
    buffer->Fill(0.5f, 10, 10);
    cout << "Output normal: " << endl;
    for(auto v : buffer->Data()){
        cout << v << endl;
    }
    buffer->RemoveLeft(10);
    cout << "Output remove left: " << endl;
    for(auto v : buffer->Data()){
        cout << v << endl;
    }
    return 0;
}