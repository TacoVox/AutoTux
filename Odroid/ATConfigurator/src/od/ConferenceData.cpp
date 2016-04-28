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













