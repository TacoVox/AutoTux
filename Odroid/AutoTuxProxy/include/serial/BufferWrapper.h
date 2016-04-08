#ifndef BUFFERWRAPPER_H
#define BUFFERWRAPPER_H

// include
// ==================================================
#include <string>
#include <vector>

// define
// ==================================================
#define BUF_LEN_MAX 1024*4


namespace serial
{
    int encode_packet(void);
    std::vector<double> decode_packet(std::string, int);

    class BufferWrapper
    {
    public:
        BufferWrapper();
        ~BufferWrapper();
        //Methods for manipulating the internal buffer representation
        void appendReceiveBuffer(std::string);
        void readReceiveBuffer(void);
        void appendSendBuffer(std::string);
        void readSendBuffer(void);
    private:
        std::string receive_buffer;
        std::string send_buffer;
        unsigned char checksum(std::vector<unsigned char>);
    };
}


#endif // BUFFERPARSER_H
