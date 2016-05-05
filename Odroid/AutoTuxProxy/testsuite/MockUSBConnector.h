/*!
 * USBConnector mock.
 *
 * @author Ivo
 */

#include "serial/USBConnector.h"
#include <gmock/gmock.h>

class MockUSBConnector : public serial::conninter::USBConnector
{
public:
    MOCK_METHOD0(connect, bool());
    MOCK_METHOD0(disconnect, bool());
    MOCK_METHOD0(read, int());
    MOCK_METHOD0(write, int());
};



