#include <iostream>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "camera/OpenCVCamera.h"

namespace proxy {
    namespace camera {
        OpenCVCamera::OpenCVCamera(const string &name, const uint32_t &id, const uint32_t &width, const uint32_t &height, const uint32_t &bpp) :
            Camera(name, id, width, height, bpp),
            m_capture(NULL),
            m_image(NULL) {
                m_capture = cvCaptureFromCAM(id);
                if(m_capture) {
                    cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_WIDTH, width);
                    cvSetCaptureProperty(m_capture, CV_CAP_PROP_FRAME_HEIGHT, height);
                } else {
                    cerr << "CameraProxy: Could not open camera '" << name << "' with ID: " << id << endl;
                }
        }

        OpenCVCamera::~OpenCVCamera() {
            if(m_capture) {
                cvReleaseCapture(&m_capture);
                m_capture = NULL;
            }
        }

        bool OpenCVCamera::isValid() const {
            return (m_capture != NULL);
        }

        bool OpenCVCamera::captureFrame() {
            bool returnVal = false;
            if(m_capture != NULL) {
                if(cvGrabFrame(m_capture)) {
                    if(getBPP() == 1) {
                        IplImage *tmpFrame = cvRetrieveFrame(m_capture);

                        if(m_image == NULL) {
                            m_image = cvCreateImage(cvGetSize(tmpFrame), IPL_DEPTH_8U, 1);
                        }

                        cvCvtColor(tmpFrame, m_image, CV_BGR2GRAY);
                    } else {
                        m_image = cvRetrieveFrame(m_capture);
                    }
                    returnVal = true;
                }
            }
            return returnVal;
        }

        bool OpenCVCamera::copyImageTo(char *dest, const uint32_t &size) {
            bool returnVal = false;

            if((dest != NULL) && (size > 0) && (m_image != NULL)) {
                memcpy(dest, m_image->imageData, size);

                cvShowImage("WindowShowImage", m_image);
                cvWaitKey(10);

                returnVal = true;
            }
            return returnVal;
        }
    }
}