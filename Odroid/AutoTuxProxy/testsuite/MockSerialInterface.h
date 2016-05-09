/*!
 * USBConnector mock.
 *
 * @author Ivo
 */

#include "serial/SerialIOInterface.h"
#include <vector>
#include <gmock/gmock.h>

using ::testing::_;

class MockSerialInterface : public serial::SerialIOInterface
{
public:
    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(disconnect, bool());
    MOCK_METHOD2(read, int(unsigned char *data, int *transferred));
    MOCK_METHOD1(write, int(std::vector<unsigned char> vec));
};



