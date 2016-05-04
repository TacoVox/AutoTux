#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "serial/USBHandler.h"
#include "MockUSBConnector.h"
#include <iostream>
#include <memory>


using namespace std;
using ::testing::AtLeast;
using ::testing::Return;


namespace {
class USBHandlerTest : public ::testing::Test
{
public:
    serial::USBHandler uh;
    testing::StrictMock<MockUSBConnector> mockUsbConn;
    //MockUSBConnector mockUsbConn;

    USBHandlerTest() :
        uh{},
        mockUsbConn{}
    {
        //uh.set_usb_connector(mockUsbConn);
    }

    //virtual ~USBHandlerTest() {}

    //virtual void SetUp() {}
    //virtual void TearDown() {}

}; // BufferWrapperTest

} // namespace


/*! */
TEST_F(USBHandlerTest, Run)
{
    EXPECT_CALL(mockUsbConn, connect())
            .Times(AtLeast(1))
            .WillOnce(Return(true));
    uh.set_usb_connector(mockUsbConn);
    uh.run();
    uh.stop();

}


int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}


