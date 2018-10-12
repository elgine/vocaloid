#pragma once
#include <memory>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>
#include "file.h"
using namespace std;
namespace vocaloid{
    struct WAV_HEADER{
        char riff[5];
        uint32_t size0;
        char wave[5];
        char fmt[5];
        uint32_t size1;
        uint16_t format_tag;
        uint16_t channels;
        uint32_t samples_per_sec;
        uint32_t bytes_per_sec;
        uint16_t block_align;
        uint16_t bits_per_sec;
        uint32_t extra_size;
        uint8_t *extra;
        char data[5];
        uint32_t size2;
    };

    class WAVWriter: public IFileWriter{
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
        explicit WAVWriter(uint32_t sample_rate, uint16_t bits, uint16_t channels){
            uint16_t block_align = bits / 8 * channels;
            uint32_t bytes_per_sec = block_align * sample_rate;
            header_ = {
                    {'R', 'I', 'F', 'F', '\0'},
                    36,
                    {'W', 'A', 'V', 'E', '\0'},
                    {'f', 'm', 't', ' ', '\0'},
                    16,
                    1,
                    channels,
                    sample_rate,
                    bytes_per_sec,
                    block_align,
                    bits,
                    0,
                    {},
                    {'d', 'a', 't', 'a', '\0'},
                    0
            };
            data_chunk_pos_ = 0;
        }

        int16_t Open(const char* output_path) override {
            out_.open(output_path, ios::binary);
            out_.write((char*)&header_.riff, 4);
            out_.write((char*)&header_.size0, sizeof header_.size0);
            out_.write((char*)&header_.wave, 4);
            out_.write((char*)&header_.fmt, 4);
            out_.write((char*)&header_.size1, sizeof header_.size1);
            out_.write((char*)&header_.format_tag, sizeof header_.format_tag);
            out_.write((char*)&header_.channels, sizeof header_.channels);
            out_.write((char*)&header_.samples_per_sec, sizeof header_.samples_per_sec);
            out_.write((char*)&header_.bytes_per_sec, sizeof header_.bytes_per_sec);
            out_.write((char*)&header_.block_align, sizeof header_.block_align);
            out_.write((char*)&header_.bits_per_sec, sizeof header_.bits_per_sec);
            out_.write((char*)&header_.data, 4);
            out_.write((char*)&header_.size2, sizeof header_.size2);
            data_chunk_pos_ = out_.tellp();
            return 0;
        }

        uint64_t WriteData(const char* bytes, uint64_t byte_length) override {
            out_.write(bytes, byte_length);
            header_.size0 += byte_length;
            header_.size2 += byte_length;
            return header_.size2;
        }

        void Close() override {
            out_.seekp(data_chunk_pos_ - 4);
            Write(out_, header_.size2, 4);
            out_.seekp(0 + 4);
            Write(out_, header_.size0, 4);
            out_.close();
        }

        WAV_HEADER GetHeader(){
            return header_;
        }
    };

    class WAVReader: public IFileReader{
    private:
        WAV_HEADER header_;
        ifstream in_;
        uint64_t pos_;
        bool has_extra_data_;
    public:
        int16_t Open(const char* source_path) override {
            has_extra_data_ = false;
            header_ = {
                    {' ', ' ', ' ', ' ', '\0'},
                    36,
                    {' ', ' ', ' ', ' ', '\0'},
                    {' ', ' ', ' ', ' ', '\0'},
                    16,
                    1,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    {},
                    {' ', ' ', ' ', ' ', '\0'},
                    0
            };
            in_.open(source_path, ios::in | ios::binary);
            in_.read((char*)&header_.riff, 4);
            if(strcmp(header_.riff, "RIFF") != 0){
                return -1;
            }
            in_.read((char*)&header_.size0, sizeof(uint32_t));
            in_.read((char*)&header_.wave, 4);
            if(strcmp(header_.wave, "WAVE") != 0){
                return -1;
            }
            in_.read((char*)&header_.fmt, 4);
            if(strcmp(header_.fmt, "fmt ") != 0){
                return -1;
            }
            in_.read((char*)&header_.size1, sizeof(uint32_t));
            in_.read((char*)&header_.format_tag, sizeof(uint16_t));
            in_.read((char*)&header_.channels, sizeof(uint16_t));
            in_.read((char*)&header_.samples_per_sec, sizeof(uint32_t));
            in_.read((char*)&header_.bytes_per_sec, sizeof(uint32_t));
            in_.read((char*)&header_.block_align, sizeof(uint16_t));
            in_.read((char*)&header_.bits_per_sec, sizeof(uint16_t));
            in_.read((char*)&header_.data, 4);
            if(strcmp(header_.data, "data") != 0){
                has_extra_data_ = true;
                in_.read((char*)&header_.extra_size, sizeof(uint32_t));
                header_.extra = new uint8_t[header_.extra_size];
                in_.read((char*)header_.extra, header_.extra_size);
                in_.read((char*)&header_.data, 4);
                if(strcmp(header_.data, "data") != 0)return -1;
                printf("%s\n", header_.extra);
            }
            in_.read((char*)&header_.size2, sizeof(uint32_t));
            pos_ = 0;
            return 0;
        }

        uint32_t GetHeaderLength(){
            uint32_t length = 44;
            if(has_extra_data_){
                length += 4 + header_.extra_size;
            }
            return length;
        }

        uint64_t Seek(uint64_t pos) override {
            if(pos < header_.size2 && pos > 0){
                pos_ = pos;
                in_.seekg(pos_ + GetHeaderLength());
            }
            return pos_;
        }

        uint64_t ReadData(char *bytes, uint64_t byte_length) override {
            if(IsEnd())return 0;
            uint64_t left = header_.size2 - pos_;
            if(left < byte_length){
                byte_length = left;
            }
            in_.read(bytes, byte_length);
            pos_ += byte_length;
            return byte_length;
        }

        bool IsEnd() override {
            return in_.eof() || header_.size2 - pos_ <= 0;
        }

        void Close() override {
            in_.close();
        }

        WAV_HEADER GetHeader(){
            return header_;
        }
    };
}