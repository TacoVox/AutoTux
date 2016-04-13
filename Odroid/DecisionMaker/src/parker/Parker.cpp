//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "parker/Parker.h"

using namespace std;

using namespace odcore::base;
using namespace odcore::base::module;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

using namespace parker;

const double INFRARED_REAR_RIGHT = 2;
enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, ENOUGHSPACE, ADJUST, PARK};
STATE state;
bool foundSpot;

bool objectFound;
double gapStart;
double gapEnd;

VehicleControl vc;

Parker::Parker(const int32_t &argc, char **argv) :
        TimeTriggeredConferenceClientModule(argc, argv, "Parker") {}

Parker::~Parker() {}

void Parker::setUp(){
    cout << "Parker starts" << endl;
    foundSpot = false;
}
void Parker::tearDown(){
    cout << "This is when Parker stops" << endl;
}

odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode Parker::body() {

    objectFound = false;
    gapStart = 0;
    gapEnd = 0;


    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {

        double absTraveled = 0;
        double parkPosition = 0;
        int parkState = 0;
        /**
         * @TODO Need to take these way later because it shall take the speed of the decisionmaker
         */
        while(*parking){
            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(automotive::VehicleData::ID());
            VehicleData vd = containerVehicleData.getData<VehicleData>();

            // 2. Get most recent sensor board data describing virtual sensor data:
            Container containerSensorBoardData = getKeyValueDataStore().get(automotive::miniature::SensorBoardData::ID());
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData>();

            absTraveled = vd.getAbsTraveledPath();

            switch (state){
                case FINDOBJECT:{
                    findObject(sbd);
                    break;
                }

                case FINDGAPSTART:{
                    findGapStart(sbd, vd);
                    break;
                }
                case FINDGAPEND:{
                    findGapEnd(sbd, vd);
                    break;
                }
                case ENOUGHSPACE:{
                    enoughSpace();
                    break;
                }
                case ADJUST:{
                    adjustCar(&absTraveled, &parkPosition);
                    break;
                }
                case PARK:{
                    parallelPark(&parkState, &absTraveled, &parkPosition);
                    break;
                }
            }
            *parkingControler = vc;

                /**
                 * @TODO Fix so that there is a safty check so that the car dosn't drive in to the back car
                 */
        }
    }
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}
void Parker::parallelPark(int* parkingState, double* absTraveled, double* parkPosition){
    switch(*parkingState){
        case 0:{
            if(*parkPosition + 5 > *absTraveled){
                vc.setSpeed(-0.6);
                vc.setSteeringWheelAngle(45);
                break;
            }
            else if(*parkPosition + 5 < *absTraveled){
                vc.setSpeed(0);
                vc.setSteeringWheelAngle(0);
                *parkingState = 1;
                break;
            }
        }
        case 1:{
            if(*parkPosition + 7 > *absTraveled){
                vc.setSpeed(-0.5);
                break;
            }
            else if(*parkPosition + 7 < *absTraveled){
                vc.setSteeringWheelAngle(-45);
                *parkingState = 2;
                break;
            }
        }
        case 2:{
            if(*parkPosition + 9.5 < *absTraveled){
                vc.setSpeed(0);
                *parkingState = 3;
                break;
            }
            else break;
        }
        case 3:{
            if(*parkPosition + 11 > *absTraveled){
                vc.setSpeed(0.3);
                vc.setSteeringWheelAngle(30);
                break;
            }
            else if(*parkPosition + 11 < *absTraveled){
                vc.setSpeed(0);
                vc.setSteeringWheelAngle(30);
                *parkingState = 4;
                break;
            }
            break;
        }
        case 4:{
            if(*parkPosition + 11.5 > *absTraveled){
                vc.setSpeed(-0.3);
                vc.setSteeringWheelAngle(-15);
            }
            else if(*parkPosition + 11.5 < *absTraveled){
                vc.setSpeed(0);
            }
        }

    }
}

void Parker::adjustCar(double* absTraveled, double* parkPosition) {
    cout << "Enters ADJUST" << endl;
    if(gapEnd + 2 > *absTraveled){
        vc.setSpeed(.6);
        vc.setSteeringWheelAngle(0);
    }
    if(gapEnd + 2 < *absTraveled){
        vc.setSpeed(0);
        state = PARK;
        *parkPosition = *absTraveled;
    }
}

void Parker::enoughSpace(){
    if((gapEnd - gapStart) > 7){
        state = ADJUST;
        foundSpot = true;
    }
    else
        state = FINDOBJECT;
}

void Parker::findGapEnd(SensorBoardData sbd, VehicleData vd){
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0) {
        cout << "Found gap END method---------------------------" << endl;
        gapEnd = vd.getAbsTraveledPath();
        state = ENOUGHSPACE;
    }
}

void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 0){
        gapStart = vd.getAbsTraveledPath();
        state = FINDGAPEND;
        cout << "Found gap START method---------------------------" << endl;
    }
}

void Parker::findObject(SensorBoardData sbd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 0){
        cout << "object found method---------------------------" << endl;
        state = FINDGAPSTART;
    }
}

/**
 * This method sets the bool parking passed from DecisionMaker
 */
void Parker::setParking(std::shared_ptr<bool> parking){
    this->parking = parking;
}
/**
 * This sets the pointer to which is the VehicleControl
 */
void Parker::setParkingControler(std::shared_ptr<Container> parkingControler){
    this->parkingControler = parkingControler;
}
/**
 * This returns a bool if there has been a parking spot found
 */
bool Parker::getFoundSpot(){
    return foundSpot;
}
