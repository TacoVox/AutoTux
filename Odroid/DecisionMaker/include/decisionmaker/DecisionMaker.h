//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_DecisionMaker_H
#define DECISIONMAKER_DecisionMaker_H

#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include <overtaker/Overtaker.h>
#include <parker/Parker.h>
#include "opendavinci/odcore/data/Container.h"

#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace decisionmaker{
    class DecisionMaker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    private:
        // "Forbidden" copy constructor
        DecisionMaker(const DecisionMaker &/*obj*/);
        //"Forbidden" assignment operator
        DecisionMaker &operator=(const DecisionMaker &/*obj*/);

    public:
        DecisionMaker(const int32_t &argc, char **argv);
        virtual ~DecisionMaker();
        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        virtual void nextContainer(odcore::data::Container &c);

    private:
        overtaker::Overtaker ovt;
        parker::Parker parker;

        virtual void setUp();
        virtual void tearDown();
        odcore::data::Container containerVehicleData;
        odcore::data::Container containerSensorBoardData;

        odcore::data::Container laneRecommendation;
        double getAngle();
        bool isInLeftLane();
        bool isDataQuality();
        double getDistanceToLine();
        void laneFollowing();
    };

} //DecisionMaker
#endif //DECISIONMAKER_DecisionMaker_H
