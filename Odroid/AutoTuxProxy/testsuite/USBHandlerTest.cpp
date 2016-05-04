
#include "serial/USBHandler.h"
#include "MockUSBConnector.h"
#include <iostream>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace std;
using ::testing::AtLeast;

/*
namespace {
class USBHandlerTest : public ::testing::Test
{
public:
    serial::USBHandler uh;
    MockUSBConnector mockUsbConn;

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
*/

/*! */
TEST(USBHandlerTest, Run)
{

}


int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}


