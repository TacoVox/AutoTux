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
        //virtual void nextContainer(odcore::data::Container &c);

    private:
        overtaker::Overtaker ovt;
        parker::Parker parker;

        virtual void setUp();
        virtual void tearDown();

        automotive::VehicleData vd;
        automotive::miniature::SensorBoardData sbd;
        autotux::DecisionMakerMSG dmMSG;
        autotux::LaneRecommendationMSG lrMSG;

        double speed;
        bool isStopLine;
        int stopCounter;

        int8_t printCounter;

        const bool NOTLEFTLANE = false;
        const bool LEFTLANE = true;

        double getAngle();
        bool isDataQuality();
        double getDistanceToLine();
        void laneFollowing();
        void printDebug();
    };

} //DecisionMaker
#endif //DECISIONMAKER_DecisionMaker_H
