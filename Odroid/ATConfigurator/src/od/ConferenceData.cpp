//
// Created by jonas on 4/28/16.
//

#include "od/ConferenceData.h"

od::ConferenceData* od::ConferenceData::_instance = 0;

od::ConferenceData::ConferenceData() { }
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