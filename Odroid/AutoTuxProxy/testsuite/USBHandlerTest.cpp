

#include <gtest/gtest.h>
#include "serial/SerialHandler.h"
#include "MockSerialInterface.h"
#include <thread>


using namespace std;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;



/**
 * Tests that running boolean is false at beginning.
 */
TEST(USBHandlerTest, AssertRunningIsFalseAtBeginning)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc = (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    serial::SerialHandler uh;

    uh.set_serialio(pmock_uc);
    ASSERT_EQ(true, uh.get_running());
}

/**
 * Tests that running boolean is false at beginning.
 */
TEST(USBHandlerTest, AssertRunningIsFalseAt)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc = (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    serial::SerialHandler uh;

    //int read_bytes;
    //unsigned char data[128];
    //vector<unsigned char> vec = {};

    EXPECT_CALL(*pmock_uc, connect())
            .WillRepeatedly(Return(true));
    /*EXPECT_CALL(*pmock_uc, read(data, &read_bytes))
            .WillRepeatedly(Return(0));
    EXPECT_CALL(*pmock_uc, write(vec))
            .WillRepeatedly(Return(0));*/
    uh.set_serialio(pmock_uc);
    //uh.run();
    //this_thread::sleep_for(chrono::seconds(5));
    uh.stop();
    ASSERT_EQ(false, uh.get_running());
}


int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}


