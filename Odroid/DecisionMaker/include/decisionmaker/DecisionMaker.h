//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_DecisionMaker_H
#define DECISIONMAKER_DecisionMaker_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

using namespace automotive;                 // Allows 'Vehicle Data'
using namespace automotive::miniature;      // Allows 'Sensor Board Data'

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
        VehicleData vd;
        SensorBoardData sbd;
        virtual void setUp();
        virtual void tearDown();

        odcore::data::Container laneRecommendation;
        double getAngle();
        bool isInLeftLane();
        bool isDataQuality();
        double getDistanceToLine();
        void laneFollowing();
    };

} //DecisionMaker
#endif //DECISIONMAKER_DecisionMaker_H
