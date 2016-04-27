/*!
 * BufferWrapper header. Declares the functions and member variables.
 *
 * @author Ivo
 */


#ifndef BUFFERWRAPPER_H
#define BUFFERWRAPPER_H

// include
// ==================================================
#include <vector>
#include <deque>

// packet values positions
#define SBDPKTSIZE      16
#define DEL_TWO_POS     1
#define DEL_DBCOLON_POS 2
#define US1_POS         3
#define US2_POS         4
#define IR1_POS         5
#define IR2_POS         6
#define IR3_POS         7
#define WHL_POS         8
#define DIS_POS_1       9
#define DIS_POS_2       10
#define DIS_POS_3       11
#define DIS_POS_4       12
#define LIGHT_SEN       13
#define CHK_SUM         14
#define DEL_COMMA_POS   15

// packet delimiters
// '1'
#define DEL_ONE         0x31
// '2'
#define DEL_TWO         0x32
// ':'
#define DEL_DBCOLON     0x3A
// ','
#define DEL_COMMA       0x2C

namespace serial
{
    class BufferWrapper
    {
    public:
        /*! constructor */
        BufferWrapper();
        /*! destructor */
        ~BufferWrapper();
        /*! appends data read from the serial to the receive buffer */
        void appendReceiveBuffer(std::vector<unsigned char>);
        /*! returns a valid packet from the receive buffer */
        std::vector<unsigned char> readReceiveBuffer(void);
        /*! appends to the send buffer data to write to the serial */
        void appendSendBuffer(std::vector<unsigned char>);
        /*! returns a valid packet to write to the serial */
        std::vector<unsigned char> readSendBuffer(void);
        /*! returns the checksum for a valid packet */
        unsigned char checksum(std::vector<unsigned char>);
    private:
        /*! the receive buffer */
        std::deque<std::vector<unsigned char>> buffer_in;
        /*! the send buffer */
        std::deque<std::vector<unsigned char>> buffer_out;
    };
}


#endif // BUFFERPARSER_H
