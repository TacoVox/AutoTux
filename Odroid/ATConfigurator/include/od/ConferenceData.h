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
        static std::unique_ptr<ConferenceData> instance(void);
    private:
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();
        static std::unique_ptr<ConferenceData> _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
