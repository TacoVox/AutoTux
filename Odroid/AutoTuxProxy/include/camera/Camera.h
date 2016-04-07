#ifndef CAMERA_H_
#define CAMERA_H_

#include <stdint.h>
#include <string>
#include <memory>

#include <opendavinci/odcore/wrapper/SharedMemory.h>
#include <opendavinci/GeneratedHeaders_OpenDaVINCI.h>

namespace proxy {
    namespace camera {
        using namespace std;

        /**
         * This class is used to wrap a camera and capture its data into a shared memory segment.
         */
        class Camera {
        private:
            /**
             * Forbidden copy constructor
             */
            Camera(const Camera &/*obj*/);

            /**
             * Forbidden assigment operator.
             */
            Camera& operator=(const Camera &/*obj*/);

        public:
            /**
             * Main Constructor
             */
            Camera(const string &name, const uint32_t &id, const uint32_t &width, const uint32_t &height, const uint32_t &bpp);

            virtual ~Camera();

            /**
             * @return Meta information about the image.
             */
            odcore::data::image::SharedImage capture();

        protected:
            /**
             * Copy the image from the specific camera (driver) to the shared memory.
             *
             * @param dest Pointer where to copy data.
             * @param size Number of bytes to copy.
             * @return true if data was successfully copied.
             */
            virtual bool copyImageTo(char *dest, const uint32_t &size) = 0;
            virtual bool captureFrame() = 0;
            virtual bool isValid() const = 0;

            const string getName() const;
            uint32_t getID() const;
            uint32_t getWidth() const;
            uint32_t getHeight() const;
            uint32_t getBPP() const;
            uint32_t getSize() const;

        private:
            odcore::data::image::SharedImage m_sharedImage;
            shared_ptr<odcore::wrapper::SharedMemory> m_sharedMemory;

        protected:
            string      m_name;
            uint32_t    m_id;
            uint32_t    m_width;
            uint32_t    m_height;
            uint32_t    m_bpp;
            uint32_t    m_size;
        };
    }
}

#endif