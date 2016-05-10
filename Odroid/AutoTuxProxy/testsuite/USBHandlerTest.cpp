

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
TEST(USBHandlerTest, AssertReadOpWithZeroReturn)
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
TEST(USBHandlerTest, AssertReadOpWithNonZeroReturn)
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
 * Tests readOp() on successful read from the serial.
 */
TEST(USBHandlerTest, AssertWriteOpWithZeroReturn)
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
 * Tests readOp() on fail read from the serial.
 */
TEST(USBHandlerTest, AssertWriteOpWithNonZeroReturn)
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
 * Tests is_reconnect() with true result.
 */
TEST(USBHandlerTest, AssertIsReconnectTrue)
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
    bool is_reconnect = serial::is_reconnect(res);

    ASSERT_EQ(true, is_reconnect);
}

/**
 * Tests is_reconnect() with true result.
 */
TEST(USBHandlerTest, AssertIsReconnectFalse)
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
            .WillOnce(Return(LIBUSB_ERROR_TIMEOUT));

    uh.set_serialio(pmock_uc);
    uh.set_buffer(sb);
    int res = uh.readOp();
    bool is_reconnect = serial::is_reconnect(res);

    ASSERT_EQ(false, is_reconnect);
}

/**
 * Tests is_reconnect() with true result.
 */
TEST(USBHandlerTest, AssertReconnectTrue)
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


int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
