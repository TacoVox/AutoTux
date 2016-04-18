//
// Created by niklas on 2016-04-07.
//
#include <iostream>

#include "opendavinci/odcore/data/Container.h"

#include "parker/Parker.h"

using namespace std;
using namespace parker;
using namespace odcore::data;
using namespace automotive;
using namespace automotive::miniature;

enum STATE {FINDOBJECT, FINDGAPSTART, FINDGAPEND, ENOUGHSPACE};
enum PARKSTATE {PHASE0,PHASE1, PHASE2, PHASE3, PHASE4, PHASE5, SAFETYSTOP};
PARKSTATE parkstate = PHASE0;
STATE state = FINDOBJECT;

double gapStart = 0;
double gapEnd = 0;
bool isSpot = false;
bool isParked = false;
VehicleControl controlTemp;

Parker::Parker(){

}

void Parker::findSpot(SensorBoardData sbd, VehicleData vd) {
    switch (state) {
        case FINDOBJECT: {
            isSpot = false;
            cout << "FIND OBJECT" << endl;
            findObject(sbd);
            break;
        }
        case FINDGAPSTART: {
            cout << "FINDG AP START" << endl;
            findGapStart(sbd, vd);
            break;
        }
        case FINDGAPEND: {
            cout << "FIND GAP END" << endl;
            findGapEnd(sbd, vd);
            break;
        }
        case ENOUGHSPACE: {
            cout << "ENOUGH SPACE" << endl;
            enoughSpace();
            break;
        }
    }
}

VehicleControl Parker::parallelPark(SensorBoardData sbd, VehicleData vd){
    cout << "This is the sensorvalue: " << sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) << endl;
    if(isNotSafe((sbd))){
        parkstate = SAFETYSTOP;
    }
    switch(parkstate){
        case PHASE0:{
            vc = adjustBeforeParking(vd, 2);
            break;
        }
        case PHASE1:{
            vc = backAroundCorner(vd, 5);
            break;
        }
        case PHASE2:{
            vc = backingStraight(vd, 2);
            break;
        }
        case PHASE3:{
            vc = backingLeft(vd, 2.5);
            break;
        }
        case PHASE4:{
            vc = adjustInSpotForward(vd, 1.5);
            break;
        }
        case PHASE5:{
            vc = adjustInSpotBack(vd, 1);
            break;
        }
        case SAFETYSTOP:{
            vc = goBackToLane(vd);
            break;
        }
    }
    return vc;
}

VehicleControl Parker::adjustInSpotBack(VehicleData vd, int add) {
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(-0.3);
        controlTemp.setSteeringWheelAngle(-15);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        isParked = true;
    }
    return controlTemp;
}

VehicleControl Parker::adjustInSpotForward(VehicleData vd, int add){
    if(carPosition + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0.3);
        controlTemp.setSteeringWheelAngle(30);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        controlTemp.setSteeringWheelAngle(30);
        carPosition = carPosition + add;
        parkstate = PHASE5;
    }
    return controlTemp;
}

VehicleControl Parker::backingLeft(VehicleData vd, int add){
    controlTemp.setSteeringWheelAngle(-45);
    if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        carPosition = carPosition + add;
        parkstate = PHASE4;
    }
    return controlTemp;
}

VehicleControl Parker::backingStraight(VehicleData vd, int add) {
    if(carPosition + add > vd.getAbsTraveledPath())
        controlTemp.setSpeed(-0.5);
    else {
        parkstate = PHASE3;
        carPosition = carPosition + add;
    }
    return controlTemp;
}

VehicleControl Parker::backAroundCorner(VehicleData vd, int add){
    if(carPosition + add > vd.getAbsTraveledPath()){
        cout << "Starts back around corner" << endl;
        controlTemp.setSpeed(-0.6);
        controlTemp.setSteeringWheelAngle(45);
    }
    else if(carPosition + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        controlTemp.setSteeringWheelAngle(0);
        carPosition = carPosition + add ;
        parkstate = PHASE2;
    }
    return controlTemp;
}
VehicleControl Parker::adjustBeforeParking(VehicleData vd, int add) {
    if(gapEnd + add > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(.6);
        controlTemp.setSteeringWheelAngle(0);
    }
    else if(gapEnd + add < vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0);
        parkstate = PHASE1;
        carPosition = vd.getAbsTraveledPath();
    }
    return controlTemp;
}

void Parker::enoughSpace(){
    if((gapEnd - gapStart) > 7){
        isSpot = true;
    }
    else
        state = FINDOBJECT;
}

void Parker::findGapEnd(SensorBoardData sbd, VehicleData vd){
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 1.2) {
        cout << "Found gap END method---------------------------" << endl;
        gapEnd = vd.getAbsTraveledPath();
        state = ENOUGHSPACE;
    }
}

void Parker::findGapStart(SensorBoardData sbd, VehicleData vd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) < 1.2){
        gapStart = vd.getAbsTraveledPath();
        state = FINDGAPEND;
        cout << "Found gap START method---------------------------" << endl;
    }
}

void Parker::findObject(SensorBoardData sbd) {
    if(sbd.getValueForKey_MapOfDistances(INFRARED_REAR_RIGHT) > 1.2){
        cout << "object found method---------------------------" << endl;
        state = FINDGAPSTART;
    }
}

bool Parker::getFoundSpot(){
    return isSpot;
}

bool Parker::getIsParked() {
    return isParked;
}

bool Parker::isNotSafe(SensorBoardData sbd){
    return((sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) > 0.5) &&
           (sbd.getValueForKey_MapOfDistances(INFRARED_REAR_BACK) < 0.8));
}

VehicleControl Parker::goBackToLane(VehicleData vd){
    if(carPosition + 4 > vd.getAbsTraveledPath()){
        controlTemp.setSpeed(0.6);
        controlTemp.setSteeringWheelAngle(-25);
    }
    else if(carPosition + 7 > vd.getAbsTraveledPath()){
        controlTemp.setSteeringWheelAngle(30);
    }
    else {
        state = FINDOBJECT;
        parkstate = PHASE0;
        isSpot = false;
    }
    return controlTemp;
}