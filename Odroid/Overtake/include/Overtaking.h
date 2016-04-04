#ifndef OVERTAKING_H_
#define OVERTAKING_H_

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

class Overtaking : public odcore::base::module::TimeTriggeredConferenceClientModule {
private:
        Overtaking(const Overtaking &/*obj*/);
        Overtaking& operator=(const Overtaking &/*obj*/);

    public:
        /**
         * Constructor.
         *
         * @param argc Number of command line arguments.
         * @param argv Command line arguments.
         */
        Overtaking(const int32_t &argc, char **argv);

        virtual ~Overtaking();

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

    private:
        virtual void setUp();

        virtual void tearDown();
        
};
