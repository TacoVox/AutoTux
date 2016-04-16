#ifndef BUFFERWRAPPER_H
#define BUFFERWRAPPER_H

// include
// ==================================================
#include <string>
#include <vector>
#include <deque>


namespace serial
{
    class BufferWrapper
    {
    public:
        BufferWrapper();
        ~BufferWrapper();
        //Methods for manipulating the internal buffer representation
        void appendReceiveBuffer(std::vector<unsigned char>);
        std::vector<unsigned char> readReceiveBuffer(void);
        void appendSendBuffer(std::vector<unsigned char>);
        std::vector<unsigned char> readSendBuffer(void);
    private:
        std::deque<std::vector<unsigned char>> buffer_in;
        std::deque<std::vector<unsigned char>> buffer_out;
        unsigned char checksum(std::vector<unsigned char>);
    };
}


#endif // BUFFERPARSER_H
