/**
 * Serial buffer header. Declares the functions and member variables.
 * Each buffer (i.e., in and out) is a deque holding vectors as elements.
 * Each packet (i.e., sensor board data and control data) is presented as a vector.
 *
 * @author Ivo
 */


#ifndef AUTOTUXPROXY_SERIALBUFFER_H
#define AUTOTUXPROXY_SERIALBUFFER_H

// include
#include <vector>
#include <deque>
#include <memory>

// packet size and values positions
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
    class SerialBuffer
    {
    public:
        /**
         * Constructor. Takes verbose mode as parameter.
         *
         * @param verbose   True for verbose, false otherwise.
         */
        SerialBuffer(bool verbose);
        /**
         * Destructor.
         */
        ~SerialBuffer();
        /**
         * Appends data read from the serial to the receive buffer.
         * Takes a vector containing the read data.
         *
         * @param v     The vector containing the stream data.
         */
        void appendReceiveBuffer(std::vector<unsigned char> v);
        /**
         * Returns a valid packet from the receive buffer. Each element
         * in the vector is an unsigned char holding a sensor board data.
         *
         * @return  A vector holding a valid sensor board data.
         */
        std::vector<unsigned char> readReceiveBuffer();
        /**
         * Appends to the send buffer control data to write to the serial.
         *
         * @param v     The vector holding the control data.
         */
        void appendSendBuffer(std::vector<unsigned char> v);
        /**
         * Returns a valid packet to write to the serial.
         *
         * @return  A vector holding valid control data.
         */
        std::vector<unsigned char> readSendBuffer();
        /**
         * Returns the checksum for a packet. Takes the vector holding
         * the sensor board data as parameter.
         *
         * @return  Checksum as unsigned char.
         */
        unsigned char checksum(const std::vector<unsigned char> v);
    private:
        /* verbose mode */
        bool verbose;
        /* the receive buffer */
        std::deque<std::vector<unsigned char>> buffer_in;
        /* the send buffer */
        std::deque<std::vector<unsigned char>> buffer_out;
    };
} // namespace serial


#endif // AUTOTUXPROXY_SERIALBUFFER_H
