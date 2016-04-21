#ifndef OPENCVCAMERA_H_
#define OPENCVCAMERA_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Camera.h"

namespace proxy {
    namespace camera {
        using namespace cv;
        using namespace std;

        /**
         * This class wraps an OpenCV camera and captures its data into a shared memory segment.
         */
        class OpenCVCamera : public Camera {
        private:
            /**
             * Forbidden copy constructor.
             */
            OpenCVCamera(const OpenCVCamera &/*obj*/);

            /**
             * Forbidden assignment operator.
             */
            OpenCVCamera& operator=(const OpenCVCamera &/*obj*/);

        public:
            /**
             * Main constructor
             *
             * @param name Name of the shared memory segment.
             * @param id OpenCVCamera identifier.
             * @param width
             * @param height
             * @param bpp
             */
            OpenCVCamera(const string &name, const uint32_t &id, const uint32_t &width, const uint32_t &height, const uint32_t &bpp);

            virtual ~OpenCVCamera();

        private:
            virtual bool copyImageTo(char *dest, const uint32_t &size);
            virtual bool isValid() const;
            virtual bool captureFrame();

        private:
            VideoCapture m_vc;
            Mat m_mat;
        };
    }
}

#endif
