//
// Created by marco on 2016-04-07.
//

#ifndef DECISIONMAKER_PARKER_H
#define DECISIONMAKER_PARKER_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace parker{
    using namespace std;

    class Parker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    private:
        Parker(const Parker &/*obj*/);
        Parker &operator=(const Parker &/*obj*/);

    public:
        Parker(const int32_t &argc, char **argv);

        virtual ~Parker();

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

    private:
        virtual void setUp();
        virtual void tearDown();
    };
} //parker
#endif //DECISIONMAKER_PARKER_H
