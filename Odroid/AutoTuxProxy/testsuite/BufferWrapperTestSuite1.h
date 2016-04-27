/*!
 *
 * @author Ivo
 */


#ifndef BUFFERWRAPPERTESTSUITE1_H
#define BUFFERWRAPPERTESTSUITE1_H

#include <cxxtest/TestSuite.h>
#include <iostream>
#include <iomanip>
#include "serial/BufferWrapper.h"

// stream length
#define STREAM_LEN      128

using namespace std;

class BufferWrapperTestSuite1 : public CxxTest::TestSuite
{
public:

    BufferWrapperTestSuite1() :
        bw(),
        valid_data(),
        invalid_data()
    {}

    /*! returns a valid stream data */
    std::vector<unsigned char> fill_valid(void) {
        std::vector<unsigned char> vec;
        for (int i = 0; i < STREAM_LEN; i += SBDPKTSIZE) {
            if (i + SBDPKTSIZE > STREAM_LEN) break;
            vec.push_back(DEL_ONE);
            vec.push_back(DEL_TWO);
            vec.push_back(DEL_DBCOLON);
            vec.push_back('2');      // us1
            vec.push_back('3');      // us2
            vec.push_back('4');      // ir1
            vec.push_back('5');      // ir2
            vec.push_back('6');      // ir3
            vec.push_back('0');      // wheel
            vec.push_back('0');      // dis1
            vec.push_back('0');      // dis2
            vec.push_back('0');      // dis3
            vec.push_back('0');      // dis4
            vec.push_back('0');      // light
            vec.push_back('6');      // checksum
            vec.push_back(DEL_COMMA);
        }
        return vec;
    }

    /*! returns an invalid stream data */
    std::vector<unsigned char> fill_invalid(unsigned char first_del,
                                            unsigned char second_del,
                                            unsigned char third_del,
                                            unsigned char checksum,
                                            unsigned char end_del) {
        std::vector<unsigned char> vec;
        for (int i = 0; i < STREAM_LEN; i += SBDPKTSIZE) {
            if (i + SBDPKTSIZE > STREAM_LEN) break;
            vec.push_back(first_del);     // wrong start delimiter
            vec.push_back(second_del);
            vec.push_back(third_del);
            vec.push_back('2');      // us1
            vec.push_back('3');      // us2
            vec.push_back('4');      // ir1
            vec.push_back('5');      // ir2
            vec.push_back('6');      // ir3
            vec.push_back('0');      // wheel
            vec.push_back('0');      // dis1
            vec.push_back('0');      // dis2
            vec.push_back('0');      // dis3
            vec.push_back('0');      // dis4
            vec.push_back('0');      // light
            vec.push_back(checksum);      // checksum (wrong)
            vec.push_back(end_del);
        }
        return vec;
    }

    void setUp() {}

    void tearDown() {}

    void testCheckSum(void) {

        std::vector<unsigned char> ret_vec;

        ret_vec.push_back('0');      // us1
        ret_vec.push_back('0');      // us2
        ret_vec.push_back('0');      // ir1
        ret_vec.push_back('0');      // ir2
        ret_vec.push_back('0');      // ir3
        ret_vec.push_back('0');      // wheel
        ret_vec.push_back('0');      // dis1
        ret_vec.push_back('0');      // dis2
        ret_vec.push_back('0');       // dis3
        ret_vec.push_back('0');       // dis4
        ret_vec.push_back('0');       // light
        TS_ASSERT_EQUALS((unsigned char) 48, bw.checksum(ret_vec));

        // clear after first test
        ret_vec.clear();

        ret_vec.push_back('2');      // us1
        ret_vec.push_back('3');      // us2
        ret_vec.push_back('4');      // ir1
        ret_vec.push_back('5');      // ir2
        ret_vec.push_back('6');      // ir3
        ret_vec.push_back('0');      // wheel
        ret_vec.push_back('0');      // dis1
        ret_vec.push_back('0');      // dis2
        ret_vec.push_back('0');       // dis3
        ret_vec.push_back('0');       // dis4
        ret_vec.push_back('0');       // light
        TS_ASSERT_EQUALS((unsigned char) 54, bw.checksum(ret_vec));
    }

    void testAppendReceiveBufferValid(void) {
        valid_data = fill_valid();
        bw.appendReceiveBuffer(valid_data);
        vector<unsigned char> v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() != 0);
        TS_ASSERT(v.at(0) == '2');
        TS_ASSERT(v.at(1) == '3');
        TS_ASSERT(v.at(2) == '4');
        TS_ASSERT(v.at(3) == '5');
        TS_ASSERT(v.at(4) == '6');

        valid_data.clear();
    }

    void testAppendReceiveBufferInvalid(void) {
        // wrong checksum
        invalid_data = fill_invalid(DEL_ONE, DEL_TWO, DEL_DBCOLON, '8', DEL_COMMA);
        bw.appendReceiveBuffer(invalid_data);
        vector<unsigned char> v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);
        // clear buffer
        invalid_data.clear();
        v.clear();
        // wrong start delimiter
        invalid_data = fill_invalid(DEL_TWO, DEL_TWO, DEL_DBCOLON, '6', DEL_COMMA);
        bw.appendReceiveBuffer(invalid_data);
        v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);
        // clear buffer
        invalid_data.clear();
        v.clear();
        // wrong second delimiter
        invalid_data = fill_invalid(DEL_ONE, DEL_ONE, DEL_DBCOLON, '6', DEL_COMMA);
        bw.appendReceiveBuffer(invalid_data);
        v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);
        // clear buffer
        invalid_data.clear();
        v.clear();
        // wrong third delimiter
        invalid_data = fill_invalid(DEL_ONE, DEL_TWO, DEL_COMMA, '6', DEL_COMMA);
        bw.appendReceiveBuffer(invalid_data);
        v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);
        // clear buffer
        invalid_data.clear();
        v.clear();
        // wrong end delimiter
        invalid_data = fill_invalid(DEL_TWO, DEL_TWO, DEL_DBCOLON, '6', DEL_DBCOLON);
        bw.appendReceiveBuffer(invalid_data);
        v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);
        // clear buffer
        invalid_data.clear();
        v.clear();
        // empty data stream
        bw.appendReceiveBuffer({});
        v = bw.readReceiveBuffer();
        TS_ASSERT(v.size() == 0);

        invalid_data.clear();
    }

    void testAppendSendBufferEmpty(void) {
        // append empty data to the send buffer
        bw.appendSendBuffer({});
        invalid_data = bw.readSendBuffer();
        TS_ASSERT(invalid_data.size() == 0);

        invalid_data.clear();
    }

private:
    serial::BufferWrapper bw;
    std::vector<unsigned char> valid_data;
    std::vector<unsigned char> invalid_data;

};

#endif	// BUFFERWRAPPERTESTSUITE1_H


