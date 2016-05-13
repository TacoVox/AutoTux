/**
 * Test suite for the serial handler. The tests are performed by
 * mocking the serial io to verify expected behaviour of the handler.
 * Googlemock documentation used as reference when creating these test suites:
 * https://github.com/google/googletest/tree/master/googlemock/docs
 *
 * @author Ivo
 */

#include <gtest/gtest.h>
#include <libusb-1.0/libusb.h>
#include "serial/SerialHandler.h"
#include "MockSerialInterface.h"



using namespace std;
using namespace serial;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;


/**
 * Tests readOp() on successful read from the serial.
 */
TEST(SerialHandlerTest, AssertReadOpWithZeroReturn)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    EXPECT_CALL(*pmock_uc, read(_, _))
            .WillOnce(Return(0));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    int res = uh.readOp();

    ASSERT_EQ(0, res);
}

/**
 * Tests readOp() on fail read from the serial.
 */
TEST(SerialHandlerTest, AssertReadOpWithNonZeroReturn)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    EXPECT_CALL(*pmock_uc, read(_, _))
            .WillOnce(Return(LIBUSB_ERROR_NO_DEVICE));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    int res = uh.readOp();

    ASSERT_EQ(LIBUSB_ERROR_NO_DEVICE, res);
}

/**
 * Tests writeOp() on successful write to the serial.
 */
TEST(SerialHandlerTest, AssertWriteOpWithZeroReturn)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    EXPECT_CALL(*pmock_uc, write(_))
            .WillOnce(Return(0));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    int res = uh.writeOp();

    ASSERT_EQ(0, res);
}

/**
 * Tests writeOp() on fail read write to the serial.
 */
TEST(SerialHandlerTest, AssertWriteOpWithNonZeroReturn)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    EXPECT_CALL(*pmock_uc, write(_))
            .WillOnce(Return(LIBUSB_ERROR_NO_DEVICE));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    int res = uh.writeOp();

    ASSERT_EQ(LIBUSB_ERROR_NO_DEVICE, res);
}

/**
 * Tests is_reconnect() with various error codes as input.
 */
TEST(SerialHandlerTest, IsReconnect)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);

    bool is_reconnect = serial::is_reconnect(LIBUSB_ERROR_IO);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_NOT_FOUND);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_NO_DEVICE);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_ACCESS);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_PIPE);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_NOT_SUPPORTED);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_OTHER);
    ASSERT_EQ(true, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_NO_MEM);
    ASSERT_EQ(false, is_reconnect);

    is_reconnect = serial::is_reconnect(LIBUSB_ERROR_INVALID_PARAM);
    ASSERT_EQ(false, is_reconnect);
}


/**
 * Tests reconnect() with true result.
 */
TEST(SerialHandlerTest, AssertReconnectTrue)
{
    // create a shared pointer, this is what we expect in the usb handler
    shared_ptr<MockSerialInterface> pmock_uc =
            (shared_ptr<MockSerialInterface>) new MockSerialInterface();
    // the buffer wrapper
    shared_ptr<SerialBuffer> sb =
            (shared_ptr<SerialBuffer>) new SerialBuffer(false);
    // the handler
    serial::SerialHandler uh;

    EXPECT_CALL(*pmock_uc, disconnect())
            .WillOnce(Return(true));
    EXPECT_CALL(*pmock_uc, connect())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(true));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    bool res = uh.reconnect();

    ASSERT_EQ(true, res);
}

/*
int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
*/