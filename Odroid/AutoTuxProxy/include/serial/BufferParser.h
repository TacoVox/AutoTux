#ifndef BUFFERPARSER_H
#define BUFFERPARSER_H

// include
// ==================================================
#include <string>
#include <vector>

// define
// ==================================================
#define BUF_LEN_MAX 1024*4


namespace buf_parser
{
    int encode_packet(void);
    std::vector<double> decode_packet(std::string, int);

    class BufferParser
    {
    public:
        BufferParser();
        ~BufferParser();
        void submit(std::string);
        void read_buffer(void);
    private:
        std::string buffer;
        unsigned char checksum(std::vector<unsigned char>);
    };
}


#endif // BUFFERPARSER_H
