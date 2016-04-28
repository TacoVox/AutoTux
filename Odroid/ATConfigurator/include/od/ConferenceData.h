//
// Created by jonas on 4/28/16.
//

#ifndef ATCONFIGURATOR_CONFERENCEDATA_H
#define ATCONFIGURATOR_CONFERENCEDATA_H

#include <vector>
#include <memory>

namespace od {
    class ConferenceData {
    public:
        static ConferenceData* instance(void);
        double getSpeed() const { return speed; }
        void setSpeed(double speed) { ConferenceData::speed = speed; }
    private:
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();
    private:
        double speed;
        static ConferenceData* _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
