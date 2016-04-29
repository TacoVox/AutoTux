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
        double getSpeed(void);
        void setSpeed(double);
    private:
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();
        double speed;
        static ConferenceData* _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
