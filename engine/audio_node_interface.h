#pragma once
#include <memory>
namespace vocaloid{
    class AudioData;
    class AudioFlow;
    class IAudioNode{
    friend class AudioFlow;
    protected:
        virtual void Process(std::shared_ptr<AudioData> in, std::shared_ptr<AudioData> out) = 0;
    };
}
