//
// Created by jonas on 4/28/16.
//

#ifndef ATCONFIGURATOR_CONFERENCEDATA_H
#define ATCONFIGURATOR_CONFERENCEDATA_H

#include <vector>
#include <memory>
#include <tiff.h>

namespace od {
    class ConferenceData {
    public:
        static ConferenceData* instance(void);
        bool isNewData(void);
        void setNewData(bool);
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
        uint32 getRoadWidth(void);
        void setRoadWidth(uint32 roadWidth);
        double getGainP(void);
        void setGainP(double gainP);
        double getGainI(void);
        void setGainI(double gainI);
        double getGainD(void);
        void setGainD(double gainD);
        uint8 getThresholdB(void);
        void setThresholdB(uint8 thresholdB);
        uint8 getThresholdD(void);
        void setThresholdD(uint8 thresholdD);
    private:
        ConferenceData();
        ConferenceData(const ConferenceData&);
        ~ConferenceData();
        bool newData;
        double speed;
        double angle;
        double us1;
        double us2;
        double ir1;
        double ir2;
        double ir3;
        uint32 roadWidth;
        double gainP;
        double gainI;
        double gainD;
        uint8 thresholdB;
        uint8 thresholdD;
        static ConferenceData* _instance;
    };
}

#endif //ATCONFIGURATOR_CONFERENCEDATA_H
