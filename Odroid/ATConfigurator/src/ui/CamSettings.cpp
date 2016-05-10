//
// Created by jonas on 5/3/16.
//

#include "ui/CamSettings.h"
#include "od/ConferenceData.h"

ui::CamSettings::CamSettings(void) { CamSettings(80, 20); }

ui::CamSettings::CamSettings(int x, int y) : xsize(x - 2), ysize(y - 2),
                                             selectedItem(0), active(false),
                                           _camsettings(newwin(ysize - 2, xsize - 16, 1, 16)) {
    mvwaddstr(_camsettings, 1, 1, "Cam Settings Test");
}

void ui::CamSettings::refresh(void) {
    //Clear the screen
    wmove(_camsettings, 0, 0);
    wclrtobot(_camsettings);
    printVals();
    wrefresh(_camsettings);
}

void ui::CamSettings::printVals(void) {
    if(selectedItem == 0 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 1, 1, "Gain P:    ");
    mvwprintw(_camsettings, 1, 9, "%g", od::ConferenceData::instance()->getGainP());
    wattroff(_camsettings, A_STANDOUT);

    if(selectedItem == 1 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 2, 1, "Gain D:    ");
    mvwprintw(_camsettings, 2, 9, "%g", od::ConferenceData::instance()->getGainD());
    wattroff(_camsettings, A_STANDOUT);

    if(selectedItem == 2 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 3, 1, "Gain I:    ");
    mvwprintw(_camsettings, 3, 9, "%g", od::ConferenceData::instance()->getGainI());
    wattroff(_camsettings, A_STANDOUT);

    if(selectedItem == 3 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 1, 20, "Road Width:     ");
    mvwprintw(_camsettings, 1, 36, "%u", od::ConferenceData::instance()->getRoadWidth());
    wattroff(_camsettings, A_STANDOUT);

    if(selectedItem == 4 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 2, 20, "Threshold Low:     ");
    mvwprintw(_camsettings, 2, 36, "%u", od::ConferenceData::instance()->getThresholdD());
    wattroff(_camsettings, A_STANDOUT);

    if(selectedItem == 5 && active)
        wattron(_camsettings, A_STANDOUT);
    mvwaddstr(_camsettings, 3, 20, "Threshold High:     ");
    mvwprintw(_camsettings, 3, 36, "%u", od::ConferenceData::instance()->getThresholdB());
    wattroff(_camsettings, A_STANDOUT);
}

void ui::CamSettings::select(void) { active = true; }

void ui::CamSettings::unselect(void) { active = false; }

void ui::CamSettings::selUp(void) {
    if(selectedItem == 0)
        selectedItem = 5;
    else
        selectedItem--;
}

void ui::CamSettings::selDn(void) {
    if(selectedItem == 5)
        selectedItem = 0;
    else
        selectedItem++;
}

void ui::CamSettings::selLeft(void) {
    if(selectedItem < 3)
        selectedItem += 3;
    else
        selectedItem -= 3;
}

void ui::CamSettings::selRight(void) {
    if(selectedItem > 2)
        selectedItem -= 3;
    else
        selectedItem += 3;
}

void ui::CamSettings::incr(void) {
    if(selectedItem == 0) {
        od::ConferenceData::instance()->setGainP(od::ConferenceData::instance()->getGainP() + 0.01);
    } else if(selectedItem == 1) {
        od::ConferenceData::instance()->setGainD(od::ConferenceData::instance()->getGainD() + 0.01);
    } else if(selectedItem == 2) {
        od::ConferenceData::instance()->setGainI(od::ConferenceData::instance()->getGainI() + 0.01);
    } else if(selectedItem == 3) {
        od::ConferenceData::instance()->setRoadWidth((uint32)(od::ConferenceData::instance()->getRoadWidth() + 1));
    } else if(selectedItem == 4) {
        od::ConferenceData::instance()->setThresholdD((uint8)(od::ConferenceData::instance()->getThresholdD() + 1));
    } else if(selectedItem == 5) {
        od::ConferenceData::instance()->setThresholdB((uint8)(od::ConferenceData::instance()->getThresholdB() + 1));
    }
}

void ui::CamSettings::decr(void) {
    if(selectedItem == 0) {
        od::ConferenceData::instance()->setGainP(od::ConferenceData::instance()->getGainP() - 0.01);
    } else if(selectedItem == 1) {
        od::ConferenceData::instance()->setGainD(od::ConferenceData::instance()->getGainD() - 0.01);
    } else if(selectedItem == 2) {
        od::ConferenceData::instance()->setGainI(od::ConferenceData::instance()->getGainI() - 0.01);
    } else if(selectedItem == 3) {
        od::ConferenceData::instance()->setRoadWidth((uint32)(od::ConferenceData::instance()->getRoadWidth() - 1));
    } else if(selectedItem == 4) {
        od::ConferenceData::instance()->setThresholdD((uint8)(od::ConferenceData::instance()->getThresholdD() - 1));
    } else if(selectedItem == 5) {
        od::ConferenceData::instance()->setThresholdB((uint8)(od::ConferenceData::instance()->getThresholdB() - 1));
    }
}