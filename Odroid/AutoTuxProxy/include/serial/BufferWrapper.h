#ifndef BUFFERWRAPPER_H
#define BUFFERWRAPPER_H

// include
// ==================================================
#include <string>
#include <vector>
#include <deque>

#define SBDPKTSIZE  10
#define DEL_POS     1
#define US1_POS     2
#define US2_POS     3
#define IR1_POS     4
#define IR2_POS     5
#define IR3_POS     6
#define WHL_POS     7
#define CHK_SUM     8
#define END_DEL     9


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
        unsigned char checksum(std::vector<unsigned char>);
    private:
        std::deque<std::vector<unsigned char>> buffer_in;
        std::deque<std::vector<unsigned char>> buffer_out;
    };
}


#endif // BUFFERPARSER_H
