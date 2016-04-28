//
// Created by jonas on 4/28/16.
//

#include "od/ConferenceData.h"

std::unique_ptr<od::ConferenceData> od::ConferenceData::_instance = 0;

od::ConferenceData::ConferenceData() { }
od::ConferenceData::ConferenceData(const ConferenceData &) { }
od::ConferenceData::~ConferenceData() { }

std::unique_ptr<od::ConferenceData> od::ConferenceData::instance(void) {
    if (!_instance)
        _instance = (std::unique_ptr<od::ConferenceData>) new od::ConferenceData();
    return _instance;
}









