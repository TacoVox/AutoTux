//
// Created by max on 4/5/16.
//

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opendavinci/odcore/wrapper/SharedMemoryFactory.h>

#include "camera/Camera.h"

namespace proxy {
    namespace camera {
        Camera::Camera(const string &name, const uint32_t &id, const uint32_t &width, const uint32_t &height, const uint32_t &bpp) :
            m_sharedImage(),
            m_sharedMemory(),
            m_name(name),
            m_id(id),
            m_width(width),
            m_height(height),
            m_bpp(bpp),
            m_size(0) {

            m_sharedMemory = odcore::wrapper::SharedMemoryFactory::createSharedMemory(name, width * height * bpp);
            m_sharedImage.setName(name);
            m_sharedImage.setWidth(width);
            m_sharedImage.setHeight(height);
            m_sharedImage.setBytesPerPixel(bpp);

            m_size = width * height * bpp;
            m_sharedImage.setSize(m_size);
        }

        Camera::~Camera() {}

        const string Camera::getName() const {
            return m_name;
        }

        uint32_t Camera::getID() const {
            return m_id;
        }

        uint32_t Camera::getWidth() const {
            return m_width;
        }

        uint32_t Camera::getHeight() const {
            return m_height;
        }

        uint32_t Camera::getBPP() const {
            return m_bpp;
        }

        uint32_t Camera::getSize() const {
            return m_size;
        }

        odcore::data::image::SharedImage Camera::capture() {
            if(isValid()) {
                if(captureFrame()) {
                    if(m_sharedMemory.get() && m_sharedMemory->isValid()) {
			cout << "locking...";
                        m_sharedMemory->lock();
                        copyImageTo((char*) m_sharedMemory->getSharedMemory(), m_size);
                        m_sharedMemory->unlock();
			cout << " unlocked." << endl;
                    }
                }
            }
            return m_sharedImage;
        }
    }
}
