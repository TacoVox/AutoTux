/*!
 * USBConnector mock.
 *
 * @author Ivo
 */

#include "serial/SerialIOInterface.h"
#include <gmock/gmock.h>

class MockSerialInterface : public serial::SerialIOInterface
{
public:
    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(disconnect, bool());
    MOCK_METHOD0(read, int());
    MOCK_METHOD0(write, int());
};



