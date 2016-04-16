//
// Created by niklas on 2016-04-05.
//

#ifndef OVERTAKING_OVERTAKE_H
#define OVERTAKING_OVERTAKE_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace over {
    namespace take {
        using namespace std;

        class Overtake : public odcore::base::module::TimeTriggeredConferenceClientModule{
        private:
            Overtake(const Overtake &/*obj*/);
            Overtake &operator=(const Overtake &/*obj*/);

        public:
            /**
             * Constructor.
             *
             * @param argc NUmber of command line arguments.
             * @param argv Command line arguments.
             */
            Overtake(const int32_t &argc, char **argv);

            virtual ~Overtake();

            odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

        private:
            virtual void setUp();
            virtual void tearDown();
        };
    }
}

#endif //OVERTAKING_OVERTAKE_H
