#include <iostream>

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

#include <Overtaking.h>

namespace over {
	namespace taking{
		using namespace std;

		using namespace odcore::base::module;
		using namespace odcore::base;
		using namespace odcore::data;

		using namespace over::taking;

		Overtaking::Overtaking(const int32_t &argc, char **argv) :
    				TimeTriggeredConferenceClientModule(argc, argv, "Overtaking"){}

		Overtaking::~Overtaking() {}

		void Overtaking::setUp() {
		}

		void Overtaking::tearDown() {
		}

		odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Overtaking::body() {
			return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
		}
	}
}
