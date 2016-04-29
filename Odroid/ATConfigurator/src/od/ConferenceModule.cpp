//
// Created by jonas on 4/28/16.
//

#include "od/ConferenceModule.h"

#include <iostream>
#include "opendavinci/odcore/data/TimeStamp.h"

using namespace std;
using namespace odcore::base::module;
using namespace odcore::data;

od::ConferenceModule::ConferenceModule(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Configurator") {}

od::ConferenceModule::~ConferenceModule() {}

void od::ConferenceModule::setUp() {}

void od::ConferenceModule::tearDown() {}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode od::ConferenceModule::body() {
    while (getModuleStateAndWaitForRemainingTimeInTimeslice() ==
            odcore::data::dmcp::ModuleStateMessage::RUNNING) {
    }

    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
