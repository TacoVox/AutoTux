#include <iostream>
#include "serial/SerialConnector.h"

using namespace std;

int main() {
    SerialConnector serial_obj;
    serial_obj.connect();
    serial_obj.read();
    return 0;
}
