#include <iostream>

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"

#include <Overtaking.h>

namespace overtaking {
	using namespace std;
	
	using namespace odcore::base::module;
	using namespace odcore::base;
	using namespace odcore::data;

	Overtaking::Overtaking(const int32_t &argc, char **argv) :
    		TimeTriggeredConferenceClientModule(argc, argv, "Overtaking"){}

	Overtaking::~Overtaking() {}

	void Overtaking::setUp() {
    	cout << "Overtaking setup starts." << endl;
	}

	void Overtaking::tearDown() {
    	cout << "Overtaking tearDown starts." << endl;
	}

	odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Overtaking::body() {
    	cout << "Tries to start moving the car" << endl;
    	while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
    		VehicleControl vc;
    		
    		vc.setSpeed(2);
    		vc.setSteeringWheelAngle(0);
    	
    		// Create container for finally sending the data.
    		Container c(vc);
    		// Send container.
    		getConference().send(c);
    
    	}
    	return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
	}
}
