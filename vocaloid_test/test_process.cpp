#include "wav_file_node.hpp"
#include "audio_context.hpp"
#include "audio_player_node.hpp"
#include "audio_gain_node.hpp"
using namespace vocaloid;

int main(){
    auto ctx = new AudioContext();
    auto wav_source = new WAVFileNode(ctx, "output1.wav");
    auto player = new AudioPlayerNode(ctx, 44100, 16, 2);
    ctx->AddNode(wav_source);
    ctx->AddNode(player);
    ctx->Connect(wav_source, player);
    return 0;
}