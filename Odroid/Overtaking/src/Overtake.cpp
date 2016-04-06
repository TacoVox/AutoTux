//
// Created by niklas on 2016-04-05.
//

#include <iostream>

#include "Overtake.h"

namespace over{
    namespace take{
        using namespace std;

        using namespace odcore::base::module;

        using namespace over::take;

        Overtake::Overtake(const int32_t &argc, char **argv) :
                TimeTriggeredConferenceClientModule(argc, argv, "Overtake") {}

        Overtake::~Overtake() {}

        void Overtake::setUp(){
            cout << "Overtake starts" << endl;
        }
        void Overtake::tearDown(){
            cout << "This is when Overtake stops" << endl;
        }

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Overtake::body() {
            int i;
            for (int i = 0; i < 10; i++) {
                cout << "This is the body" << endl;
            }

            return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
        }
    }
}