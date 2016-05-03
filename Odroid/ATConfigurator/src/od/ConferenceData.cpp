//
// Created by jonas on 4/28/16.
//

#include "od/ConferenceData.h"

od::ConferenceData* od::ConferenceData::_instance = 0;

od::ConferenceData::ConferenceData() : speed(0.0), angle(90.0), us1(0.0),
                                       us2(0.0), ir1(0.0), ir2(0.0), ir3(0.0),
                                       roadWidth(220), gainP(0.9), gainI(0.0),
                                       gainD(0.0), thresholdB(200),
                                       thresholdD(50) { }
od::ConferenceData::ConferenceData(const ConferenceData &) { }
od::ConferenceData::~ConferenceData() { }

od::ConferenceData* od::ConferenceData::instance(void) {
    if (!_instance)
        _instance = new od::ConferenceData();
    return _instance;
}

double od::ConferenceData::getSpeed(void) { return this->speed; }

void od::ConferenceData::setSpeed(double s) { this->speed = s; }

double od::ConferenceData::getAngle(void) { return this->angle; }

void od::ConferenceData::setAngle(double a) { this->angle = a; }

double od::ConferenceData::getUS1(void) { return this->us1; }

void od::ConferenceData::setUS1(double us) { this->us1 = us; }

double od::ConferenceData::getUS2(void) { return this->us2; }

void od::ConferenceData::setUS2(double us) { this->us2 = us; }

double od::ConferenceData::getIR1(void) { return this->ir1; }

void od::ConferenceData::setIR1(double ir) { this->ir1 = ir; }

double od::ConferenceData::getIR2(void) { return this->ir2; }

void od::ConferenceData::setIR2(double ir) { this->ir2 = ir; }

double od::ConferenceData::getIR3(void) { return this->ir3; }

void od::ConferenceData::setIR3(double ir) { this->ir3 = ir; }

uint32 od::ConferenceData::getRoadWidth() { return this->roadWidth; }

void od::ConferenceData::setRoadWidth(uint32 rw) { this->roadWidth = rw; }

double od::ConferenceData::getGainP() { return this->gainP; }

void od::ConferenceData::setGainP(double gp) { this->gainP = gp; }

double od::ConferenceData::getGainI() { return this->gainI; }

void od::ConferenceData::setGainI(double gi) { this->gainI = gi; }

double od::ConferenceData::getGainD() { return this->gainD; }

void od::ConferenceData::setGainD(double gd) { this->gainD = gd; }

uint8 od::ConferenceData::getThresholdB() { return this->thresholdB; }

void od::ConferenceData::setThresholdB(uint8 tb) { this->thresholdB = tb; }

uint8 od::ConferenceData::getThresholdD() { return this->thresholdD; }

void od::ConferenceData::setThresholdD(uint8 td) { this->thresholdD = td; }