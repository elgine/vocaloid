#include <audio_frame.hpp>
#include <audio_port.hpp>
using namespace vocaloid;

int main(int argc, char** argv) {
    AudioPort port;
    AudioFrame f1(1024), f2(1024, 4), f3(1024, 6);
    f1.Fill(0.5);
    f2.Fill(0.2);
    port.AddBuffer(0);
    port.PushFrame(0, &f1);
    port.AddBuffer(1);
    port.PushFrame(1, &f2);
    port.GetFrame(&f3);
    return 0;
}
