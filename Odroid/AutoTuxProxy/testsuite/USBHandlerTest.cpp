

#include <gtest/gtest.h>
#include "serial/USBHandler.h"
#include "MockUSBConnector.h"
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
    shared_ptr<MockUSBConnector> pmock_uc = (shared_ptr<MockUSBConnector>) new MockUSBConnector();
    serial::handler::USBHandler uh;

    uh.set_usb_connector(pmock_uc);
    ASSERT_EQ(false, uh.get_running());
}

/**
 * Tests that running boolean is false at beginning.
 */
TEST(USBHandlerTest, AssertRunningIsFalseAt)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockUSBConnector> pmock_uc = (shared_ptr<MockUSBConnector>) new MockUSBConnector();
    serial::handler::USBHandler uh;

    EXPECT_CALL(*pmock_uc, connect())
            .WillRepeatedly(Return(true));
    uh.set_usb_connector(pmock_uc);
    //uh.run();
    uh.stop();
    ASSERT_EQ(false, uh.get_running());
}


int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}


