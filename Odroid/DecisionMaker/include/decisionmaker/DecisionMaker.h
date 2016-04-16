//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_DecisionMaker_H
#define DECISIONMAKER_DecisionMaker_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace decisionmaker{
    class DecisionMaker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    public:
        DecisionMaker(const int32_t &argc, char **argv);
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        virtual void nextContainer(odcore::data::Container &c);
        virtual ~DecisionMaker();
    private:



    private:
        //DecisionMaker(const DecisionMaker &/*obj*/);
        //DecisionMaker &operator=(const DecisionMaker &/*obj*/);
        virtual void setUp();
        virtual void tearDown();

        const double ULTRASONIC_FRONT_RIGHT = 4;
        const double ULTRASONIC_FRONT_FORWARD = 3;
        const double INFRARED_FRONT_RIGHT = 0;
        const double INFRARED_REAR_RIGHT = 2;

        //automotive::VehicleData vd;
        //automotive::miniature::SensorBoardData sbd;

        odcore::data::Container laneRecommendation;
        double getAngle();
        bool isInLeftLane();
        bool isDataQuality();
        double getDistanceToLine();
        void laneFollowing();
    };

} //DecisionMaker
#endif //DECISIONMAKER_DecisionMaker_H
