//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_DecisionMaker_H
#define DECISIONMAKER_DecisionMaker_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>

namespace decisionmaker{
    using namespace std;

    class DecisionMaker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    private:
        DecisionMaker(const DecisionMaker &/*obj*/);
        DecisionMaker &operator=(const DecisionMaker &/*obj*/);

    public:
        DecisionMaker(const int32_t &argc, char **argv);

        virtual ~DecisionMaker();

        virtual void nextContainer(odcore::data::Container &c);

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();

    private:
        virtual void setUp();
        virtual void tearDown();
    };

} //DecisionMaker
#endif //DECISIONMAKER_DecisionMaker_H
