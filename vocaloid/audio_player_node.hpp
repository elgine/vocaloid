#pragma once
#include <memory>
#include "audio_destination_node.hpp"
#include "pcm_player.h"
#include "audio_context_interface.h"
namespace vocaloid{

    class AudioPlayerNode: public AudioDestinationNode{
    private:
        PCMPlayer *player_;
        uint32_t sample_rate_;
        uint16_t bits_;
        uint16_t channels_;
    public:
        explicit AudioPlayerNode(IAudioContext *ctx,
                                    uint32_t sample_rate,
                                    uint16_t bits,
                                    uint16_t channels):AudioDestinationNode(ctx),
                                                        sample_rate_(sample_rate),
                                                        bits_(bits),
                                                        channels_(channels){
            player_ = new PCMPlayer();
        }

        int16_t Prepare(){
            player_->Open(sample_rate_, bits_, channels_);
        }

        void Process(Buffer *in) {
            uint64_t buf_len = in->GetBufferSize() * in->GetChannels() * in->GetBits() / 8;
            std::unique_ptr<char*> buf = std::make_unique<char*>(new char[buf_len]);
            player_->Push(*buf, buf_len);
        }
    };
}