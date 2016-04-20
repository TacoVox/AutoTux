#ifndef BUFFERWRAPPER_H
#define BUFFERWRAPPER_H

// include
// ==================================================
#include <string>
#include <vector>
#include <deque>

// packet values positions
#define SBDPKTSIZE      10
#define MID_DEL_POS     1
#define US1_POS         2
#define US2_POS         3
#define IR1_POS         4
#define IR2_POS         5
#define IR3_POS         6
#define WHL_POS         7
#define CHK_SUM         8
#define END_DEL_POS     9

// packet delimiters
#define STR_DEL         0x37
#define MID_DEL         0x3A
#define END_DEL         0x2C

namespace serial
{
    class BufferWrapper
    {
    public:
        BufferWrapper();
        ~BufferWrapper();
        void appendReceiveBuffer(std::vector<unsigned char>);
        std::vector<unsigned char> readReceiveBuffer(void);
        void appendSendBuffer(std::vector<unsigned char>);
        std::vector<unsigned char> readSendBuffer(void);
    private:
        unsigned char checksum(std::vector<unsigned char>);
    private:
        std::deque<std::vector<unsigned char>> buffer_in;
        std::deque<std::vector<unsigned char>> buffer_out;
    };
}


#endif // BUFFERPARSER_H
