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
        double getAngle(void);
        void setAngle(double);
        double getUS1(void);
        void setUS1(double);
        double getUS2(void);
        void setUS2(double);
        double getIR1(void);
        void setIR1(double);
        double getIR2(void);
        void setIR2(double);
        double getIR3(void);
        void setIR3(double);
    private:
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();
        double speed;
        double angle;
        double us1;
        double us2;
        double ir1;
        double ir2;
        double ir3;
        static ConferenceData* _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
