//
// Created by jonas on 4/28/16.
//

#ifndef ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H
#define ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace od {
    class ConferenceModule
            : public odcore::base::module::TimeTriggeredConferenceClientModule {
    public:
        ConferenceModule(const int32_t &argc, char **argv);
        virtual ~ConferenceModule();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
    private:
        ConferenceModule(const ConferenceModule &);
        ConferenceModule& operator=(const ConferenceModule &);
        virtual void setUp();
        virtual void tearDown();
    };
}

#endif //ATCONFIGURATOR_CLIENTCONFERENCEMODULE_H
