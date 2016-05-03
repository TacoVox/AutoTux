
#include "serial/BufferWrapper.h"
#include <iostream>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


#define STREAM_LEN 128

using namespace std;

namespace {

// The fixture for testing class BufferWrapperTest.
class BufferWrapperTest : public ::testing::Test
{
public:
    // Objects declared here can be used by all tests.
    unique_ptr<serial::BufferWrapper> bw;
    std::vector<unsigned char> valid_data;
    std::vector<unsigned char> invalid_data;
    std::vector<unsigned char> test_vec;

    BufferWrapperTest() :
        bw(new serial::BufferWrapper()),
        valid_data(),
        invalid_data(),
        test_vec()
    {}

    //virtual ~BufferWrapperTest(){}

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    //virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
    //}

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
        valid_data.clear();
        invalid_data.clear();
        test_vec.clear();
    }

    /*! returns a valid stream data */
    std::vector<unsigned char> fill_valid(void) {
        for (int i = 0; i < STREAM_LEN; i += SBDPKTSIZE) {
            if (i + SBDPKTSIZE > STREAM_LEN) break;
            valid_data.push_back(DEL_ONE);
            valid_data.push_back(DEL_TWO);
            valid_data.push_back(DEL_DBCOLON);
            valid_data.push_back('2');      // us1
            valid_data.push_back('3');      // us2
            valid_data.push_back('4');      // ir1
            valid_data.push_back('5');      // ir2
            valid_data.push_back('6');      // ir3
            valid_data.push_back('0');      // wheel
            valid_data.push_back('0');      // dis1
            valid_data.push_back('0');      // dis2
            valid_data.push_back('0');      // dis3
            valid_data.push_back('0');      // dis4
            valid_data.push_back('0');      // light
            valid_data.push_back('6');      // checksum
            valid_data.push_back(DEL_COMMA);
        }
        return valid_data;
    }

    /*! returns an invalid stream data */
    std::vector<unsigned char> fill_invalid(unsigned char first_del,
                                            unsigned char second_del,
                                            unsigned char third_del,
                                            unsigned char checksum,
                                            unsigned char end_del) {
        for (int i = 0; i < STREAM_LEN; i += SBDPKTSIZE) {
            if (i + SBDPKTSIZE > STREAM_LEN) break;
            invalid_data.push_back(first_del);
            invalid_data.push_back(second_del);
            invalid_data.push_back(third_del);
            invalid_data.push_back('2');      // us1
            invalid_data.push_back('3');      // us2
            invalid_data.push_back('4');      // ir1
            invalid_data.push_back('5');      // ir2
            invalid_data.push_back('6');      // ir3
            invalid_data.push_back('0');      // wheel
            invalid_data.push_back('0');      // dis1
            invalid_data.push_back('0');      // dis2
            invalid_data.push_back('0');      // dis3
            invalid_data.push_back('0');      // dis4
            invalid_data.push_back('0');      // light
            invalid_data.push_back(checksum);
            invalid_data.push_back(end_del);
        }
        return invalid_data;
    }

}; // BufferWrapperTest

/*! tests that checksum works in buffer wrapper */
TEST_F(BufferWrapperTest, CheckSum) {

    test_vec.push_back('0');      // us1
    test_vec.push_back('0');      // us2
    test_vec.push_back('0');      // ir1
    test_vec.push_back('0');      // ir2
    test_vec.push_back('0');      // ir3
    test_vec.push_back('0');      // wheel
    test_vec.push_back('0');      // dis1
    test_vec.push_back('0');      // dis2
    test_vec.push_back('0');       // dis3
    test_vec.push_back('0');       // dis4
    test_vec.push_back('0');       // light

    ASSERT_EQ((unsigned char) 48, bw->checksum(test_vec));

    // clear after first test
    test_vec.clear();

    test_vec.push_back('2');      // us1
    test_vec.push_back('3');      // us2
    test_vec.push_back('4');      // ir1
    test_vec.push_back('5');      // ir2
    test_vec.push_back('6');      // ir3
    test_vec.push_back('0');      // wheel
    test_vec.push_back('0');      // dis1
    test_vec.push_back('0');      // dis2
    test_vec.push_back('0');       // dis3
    test_vec.push_back('0');       // dis4
    test_vec.push_back('0');       // light

    ASSERT_EQ((unsigned char) 54, bw->checksum(test_vec));
}


/*! tests append valid data stream to receive buffer */
/*
TEST_F(BufferWrapperTest, AppendValidToReceiveBuffer) {
    valid_data = fill_valid();
    bw.appendReceiveBuffer(valid_data);
    std::vector<unsigned char> v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() != 0);
    ASSERT_TRUE(v.at(0) == '2');
    ASSERT_TRUE(v.at(1) == '3');
    ASSERT_TRUE(v.at(2) == '4');
    ASSERT_TRUE(v.at(3) == '5');
    ASSERT_TRUE(v.at(4) == '6');
}
*/

/*! tests append invalid data stream to receive buffer */
/*
TEST_F(BufferWrapperTest, AppendInvalidToReceiveBuffer) {

    // wrong checksum
    invalid_data = fill_invalid(DEL_ONE, DEL_TWO, DEL_DBCOLON, '8', DEL_COMMA);
    bw.appendReceiveBuffer(invalid_data);
    std::vector<unsigned char> v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
    // clear
    invalid_data.clear();
    v.clear();

    // wrong start delimiter
    invalid_data = fill_invalid(DEL_TWO, DEL_TWO, DEL_DBCOLON, '6', DEL_COMMA);
    bw.appendReceiveBuffer(invalid_data);
    v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
    // clear buffer
    invalid_data.clear();
    v.clear();

    // wrong second delimiter
    invalid_data = fill_invalid(DEL_ONE, DEL_ONE, DEL_DBCOLON, '6', DEL_COMMA);
    bw.appendReceiveBuffer(invalid_data);
    v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
    // clear buffer
    invalid_data.clear();
    v.clear();

    // wrong third delimiter
    invalid_data = fill_invalid(DEL_ONE, DEL_TWO, DEL_COMMA, '6', DEL_COMMA);
    bw.appendReceiveBuffer(invalid_data);
    v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
    // clear buffer
    invalid_data.clear();
    v.clear();

    // wrong end delimiter
    invalid_data = fill_invalid(DEL_TWO, DEL_TWO, DEL_DBCOLON, '6', DEL_DBCOLON);
    bw.appendReceiveBuffer(invalid_data);
    v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
    // clear buffer
    invalid_data.clear();
    v.clear();

    // empty data stream
    bw.appendReceiveBuffer({});
    v = bw.readReceiveBuffer();
    ASSERT_TRUE(v.size() == 0);
}
*/

/*! tests appending empty data to send buffer */
/*
TEST_F(BufferWrapperTest, AppendEmptyToSendBuffer) {
    bw.appendSendBuffer({});
    invalid_data = bw.readSendBuffer();
    ASSERT_TRUE(invalid_data.size() == 0);
}
*/

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

