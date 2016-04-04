#ifndef LANEDETECTOR_H_
#define LANEDETECTOR_H_

#include <memory>

#include <opencv/cv.h>
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <opendavinci/odcore/data/TimeStamp.h>
#include <opendavinci/odcore/wrapper/SharedMemory.h>

namespace lane {
    namespace detector {
        using namespace std;

        class LaneDetector: public odcore::base::module::TimeTriggeredConferenceClientModule {
        private:
            /**
             * Forbidden.
             *
             * TODO Describe why
             *
             * @param obj References an object of this class.
             */
            LaneDetector(const LaneDetector &/*obj*/);

            /**
             * Forbidden
             *
             * TODO Describe why
             *
             * @param obj References an object of this class.
             * @return Reference to this instance.
             */
            LaneDetector& operator=(const LaneDetector &/*obj*/);

        public:
            /**
             * Main constructor
             */
            LaneDetector(const int32_t &argc, char **argv);

            virtual ~LaneDetector();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        protected:
            /**
             * This method is called to process incoming containers
             *
             * @param c Container to process.
             * @return true if successfully processed.
             */
            bool readSharedImage(odcore::data::Container &c);
            int m_distToLeftMarking;
            int m_distToRightMarking;

        private:
            bool m_hasAttachedToSharedImageMemory;
            bool m_debug;

            CvFont m_font;
            std::shared_ptr<odcore::wrapper::SharedMemory> m_sharedImageMemory;
            odcore::data::TimeStamp m_previousTime;
            IplImage *m_image;


            virtual void setUp();
            virtual void tearDown();

            void processImage();
        };
    } // detector
} // lane


#endif
