
#include "serial/BufferWrapper.h"
#include "serial/USBConnector.h"

namespace usb_handler {
    class USBHandler
    {
    public:
        USBHandler(std::shared_ptr<serial::BufferWrapper>);
        ~USBHandler();
        void run();
        void set_buffer_wrapper(std::shared_ptr<serial::BufferWrapper>);
    private:
        std::shared_ptr<serial::BufferWrapper> bw;
        usb_connector::USBConnector uc;
        void reconnect();
    };
} // namespace usb_handler
