#include <wav_audio_file_node.hpp>
#include <audio_context.hpp>
#include <audio_player_node.hpp>
#include <audio_gain_node.hpp>
using namespace vocaloid;
int main(){
    auto ctx = new AudioContext();
    auto wav_source = new WAVAudioFileNode(ctx, "output1.wav");
    auto player = new AudioPlayerNode(ctx, 44100, 16, 2);
    wav_source->Connect(player);
    return 0;
}