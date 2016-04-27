//
// Created by niklas on 2016-04-07.
//

#ifndef DECISIONMAKER_PARKER_H
#define DECISIONMAKER_PARKER_H

#include "opendavinci/odcore/data/Container.h"
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"

namespace parker {
    using namespace std;

    class Parker {

    private:

        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;

        double carPosition;

        automotive::VehicleControl vc;

        void findObject(automotive::miniature::SensorBoardData);
        void findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapEnd(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void enoughSpace();
        automotive::VehicleControl adjustBeforeParking(automotive::VehicleData, int);
        automotive::VehicleControl backAroundCorner(automotive::VehicleData, int);
        automotive::VehicleControl backingStraight(automotive::VehicleData, int);
        automotive::VehicleControl backingLeft(automotive::VehicleData, int);
        automotive::VehicleControl adjustInSpotForward(automotive::VehicleData, int);
        automotive::VehicleControl adjustInSpotBack(automotive::VehicleData, int);

        bool isNotSafe(automotive::miniature::SensorBoardData);
        automotive::VehicleControl goBackToLane(automotive::VehicleData);

    public:
        Parker();

        void findSpot(automotive::miniature::SensorBoardData, automotive::VehicleData);
        bool getFoundSpot();
        bool getIsParked();

        automotive::VehicleControl parallelPark(automotive::miniature::SensorBoardData, automotive::VehicleData);
    };
}


/*
#include <opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h>
#include "automotivedata/GeneratedHeaders_AutomotiveData.h"
#include "opendavinci/GeneratedHeaders_OpenDaVINCI.h"
#include "opendavinci/odcore/data/Container.h"

namespace parker{
    using namespace std;

    class Parker : public odcore::base::module::TimeTriggeredConferenceClientModule{
    private:
        Parker(const Parker &/*obj*///);
  //      Parker &operator=(const Parker &/*obj*/);
/*
        std::shared_ptr<bool> parking;
        std::shared_ptr<odcore::data::Container> parkingControler;

        bool findParkingSpot(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapEnd(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findObject(automotive::miniature::SensorBoardData);
        void enoughSpace();
        void adjustCar(double);
        void parallelPark();
        void backAroundObj();
        void backingStraight();
        void backingLeft();
        void adjustInSpotForward();
        void adjustInSpotBack();
        void goBackToLane();
        bool isSafe(automotive::miniature::SensorBoardData);

        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;
        enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, ENOUGHSPACE, ADJUST, PARK, SAFETYSTOP};
        enum PARKSTATE {PHASE1, PHASE2, PHASE3, PHASE4, PHASE5};
        STATE state;
        PARKSTATE parkstate;
        bool foundSpot;

        double gapStart;
        double gapEnd;
        double absTraveled;
        double parkPosition;

        automotive::VehicleControl vc;



    public:
        Parker(const int32_t &argc, char **argv);

        virtual ~Parker();

        odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
        bool getFoundSpot();
        void setParking(std::shared_ptr<bool>);
        void setParkingControler(std::shared_ptr<odcore::data::Container>);

    private:
        virtual void setUp();
        virtual void tearDown();
    };
} //parker
*/
#endif //DECISIONMAKER_PARKER_H
