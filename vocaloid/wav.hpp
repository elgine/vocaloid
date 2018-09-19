#pragma once
#include <memory>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

namespace vocaloid{

    namespace wav{

        struct WAV_HEADER{
            char riff[4];
            uint32_t size0;
            char wave[4];
            char fmt[4];
            uint32_t size1;
            uint16_t format_tag;
            uint16_t channels;
            uint32_t samples_per_sec;
            uint32_t bytes_per_sec;
            uint16_t block_align;
            uint16_t bits_per_sec;
            char data[4];
            uint32_t size2;
        };

        class Writer{
        private:
            WAV_HEADER header_;
            ofstream out_;
            size_t data_chunk_pos_;

            template <typename Word>
            std::ostream& Write( std::ostream& outs, Word value, unsigned size = sizeof( Word ) ) {
                for (; size; --size, value >>= 8)
                    outs.put( static_cast <char> (value & 0xFF) );
                return outs;
            }

        public:
            Writer(uint32_t sample_rate, uint16_t bits, uint16_t channels){
                uint16_t block_align = bits / 8 * channels;
                uint32_t bytes_per_sec = block_align * sample_rate;
                header_ = {
                    {'R', 'I', 'F', 'F'},
                    36,
                    {'W', 'A', 'V', 'E'},
                    {'f', 'm', 't', ' '},
                    16,
                    1,
                    channels,
                    sample_rate,
                    bytes_per_sec,
                    block_align,
                    bits,
                    {'d', 'a', 't', 'a'},
                    0
                };
                data_chunk_pos_ = 0;
            }

            uint16_t Open(string output_path){
                out_.open(output_path.c_str(), ios::binary);
                out_.write((char*)&header_, sizeof header_);
                data_chunk_pos_ = out_.tellp();
                return 0;
            }

            uint64_t WritePCMData(uint8_t* bytes, uint64_t byte_length){
                for(int i = 0;i < byte_length;i++){
                    out_.put(bytes[i]);
                }
                header_.size0 += byte_length;
                header_.size2 += byte_length;
                return header_.size2;
            }

            void Close(){
                out_.seekp(data_chunk_pos_ - 4);
                Write(out_, header_.size2, 4);
                out_.seekp(0 + 4);
                Write(out_, header_.size0, 4);
                out_.close();
            }
        };
    };
}