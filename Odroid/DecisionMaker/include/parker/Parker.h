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

        //REAL CAR
        const double SPOT_SIZE = 0.70;
        const double ADJUST_BEFORE_PARKING = 0.10;
        const double BACK_AROUND_CORNER = 0.30;
        const double BACKING_STRAIGHT = 0.20;
        const double BACKING_LEFT = 0.10;
        const double ADJUST_IN_SPOT_FORWARD = 0.10;
        const double ADJUST_IN_SPOT_BACK = 0.10;

        /*
        //SIMULATION
        const double SPOT_SIZE = 7;
        const double ADJUST_BEFORE_PARKING = 2;
        const double BACK_AROUND_CORNER = 5;
        const double BACKING_STRAIGHT = 2;
        const double BACKING_LEFT = 2.5;
        const double ADJUST_IN_SPOT_FORWARD = 1.5;
        const double ADJUST_IN_SPOT_BACK = 1;
        //----------------------------------------
        */

        const double INFRARED_REAR_RIGHT = 2;
        const double INFRARED_REAR_BACK = 1;

        double carPosition;

        automotive::VehicleControl vc;

        void findObject(automotive::miniature::SensorBoardData);
        void findGapStart(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void findGapEnd(automotive::miniature::SensorBoardData, automotive::VehicleData);
        void enoughSpace();
        automotive::VehicleControl adjustBeforeParking(automotive::VehicleData, double);
        automotive::VehicleControl backAroundCorner(automotive::VehicleData, double);
        automotive::VehicleControl backingStraight(automotive::VehicleData, double);
        automotive::VehicleControl backingLeft(automotive::VehicleData, double);
        automotive::VehicleControl adjustInSpotForward(automotive::VehicleData, double);
        automotive::VehicleControl adjustInSpotBack(automotive::VehicleData, double);

        bool isNotSafe(automotive::miniature::SensorBoardData);
        automotive::VehicleControl goBackToLane(automotive::VehicleData);

    public:
        Parker();
        virtual ~Parker();
        void findSpot(automotive::miniature::SensorBoardData, automotive::VehicleData);
        bool getFoundSpot();
        bool getIsParked();

        automotive::VehicleControl parallelPark(automotive::miniature::SensorBoardData, automotive::VehicleData);
    };
}
#endif //DECISIONMAKER_PARKER_H
