//
// Created by marco on 2016-04-07.
//

//
// Created by niklas on 2016-04-05.
//

#ifndef OVERTAKING_Parking_H
#define OVERTAKING_Parking_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace over {
    namespace take {
        using namespace std;

        class Parking : public odcore::base::module::TimeTriggeredConferenceClientModule{
        private:
            Parking(const Parking &/*obj*/);
            Parking &operator=(const Parking &/*obj*/);

        public:
            /**
             * Constructor.
             *
             * @param argc NUmber of command line arguments.
             * @param argv Command line arguments.
             */
            Parking(const int32_t &argc, char **argv);

            virtual ~Parking();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        private:
            virtual void setUp();
            virtual void tearDown();
        };
    }
}

#endif //OVERTAKING_Parking_H
