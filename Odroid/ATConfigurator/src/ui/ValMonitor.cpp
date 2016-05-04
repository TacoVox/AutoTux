//
// Created by jonas on 4/26/16.
//

#include "ui/ValMonitor.h"
#include "od/ConferenceData.h"

ui::ValMonitor::ValMonitor(void) { ValMonitor(80, 20); }

ui::ValMonitor::ValMonitor(int x, int y) : xsize(x - 2), ysize(y - 2),
                                       _valmonitor(newwin(ysize - 2, xsize - 16,
                                                          1, 16)) {
    mvwaddstr(_valmonitor, 1, 1, "Val Monitor Test");
}

void ui::ValMonitor::refresh(void) {
    printVals();
    wrefresh(_valmonitor);
}

void ui::ValMonitor::printVals(void) {
    //Clear the screen
    wmove(_valmonitor, 1, 1);
    wclrtobot(_valmonitor);

    //Current steering stuff
    mvwaddstr(_valmonitor, 1, 1, "Speed:");
    mvwprintw(_valmonitor, 1, 8, "%g", od::ConferenceData::instance()->getSpeed());

    mvwaddstr(_valmonitor, 2, 1, "Angle:");
    mvwprintw(_valmonitor, 2, 8, "%g", od::ConferenceData::instance()->getAngle());

    //Current sensor values
    mvwaddstr(_valmonitor, 1, 18, "US1:");
    mvwprintw(_valmonitor, 1, 23, "%g", od::ConferenceData::instance()->getUS1());

    mvwaddstr(_valmonitor, 2, 18, "US2:");
    mvwprintw(_valmonitor, 2, 23, "%g", od::ConferenceData::instance()->getUS2());

    mvwaddstr(_valmonitor, 3, 18, "IR1:");
    mvwprintw(_valmonitor, 3, 23, "%g", od::ConferenceData::instance()->getIR1());

    mvwaddstr(_valmonitor, 4, 18, "IR2:");
    mvwprintw(_valmonitor, 4, 23, "%g", od::ConferenceData::instance()->getIR2());

    mvwaddstr(_valmonitor, 5, 18, "IR3:");
    mvwprintw(_valmonitor, 5, 23, "%g", od::ConferenceData::instance()->getIR3());

    mvwaddstr(_valmonitor, 1, 35, "Angle:");
    mvwprintw(_valmonitor, 1, 50, "%g", od::ConferenceData::instance()->getRec_angle());

    mvwaddstr(_valmonitor, 2, 35, "Dist. to line:");
    mvwprintw(_valmonitor, 2, 50, "%g", od::ConferenceData::instance()->getDistance_to_line());

    mvwaddstr(_valmonitor, 3, 35, "Quality:");
    if(od::ConferenceData::instance()->isQuality())
        mvwaddstr(_valmonitor, 3, 50, "true");
    else
        mvwaddstr(_valmonitor, 3, 50, "false");
}

//Do nothing for now
void ui::ValMonitor::select(void) {}
void ui::ValMonitor::unselect(void) {}
void ui::ValMonitor::selUp(void) {}
void ui::ValMonitor::selDn(void) {}
void ui::ValMonitor::selLeft(void) {}
void ui::ValMonitor::selRight(void) {}
void ui::ValMonitor::incr(void) {}
void ui::ValMonitor::decr(void) {}