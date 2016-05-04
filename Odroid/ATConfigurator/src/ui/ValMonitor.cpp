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
    mvwaddstr(_valmonitor, 1, 20, "US1:");
    mvwprintw(_valmonitor, 1, 25, "%g", od::ConferenceData::instance()->getUS1());

    mvwaddstr(_valmonitor, 2, 20, "US2:");
    mvwprintw(_valmonitor, 2, 25, "%g", od::ConferenceData::instance()->getUS2());

    mvwaddstr(_valmonitor, 3, 20, "IR1:");
    mvwprintw(_valmonitor, 3, 25, "%g", od::ConferenceData::instance()->getIR1());

    mvwaddstr(_valmonitor, 4, 20, "IR2:");
    mvwprintw(_valmonitor, 4, 25, "%g", od::ConferenceData::instance()->getIR2());

    mvwaddstr(_valmonitor, 5, 20, "IR3:");
    mvwprintw(_valmonitor, 5, 25, "%g", od::ConferenceData::instance()->getIR3());
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