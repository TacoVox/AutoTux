/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#include "ui/ValMonitor.h"
#include "od/ConferenceData.h"

ui::ValMonitor::ValMonitor(void) { ValMonitor(80, 20); }

ui::ValMonitor::ValMonitor(int x, int y) : xsize(x - 2), ysize(y - 2),
                                       _valmonitor(newwin(ysize - 2, xsize - 16,
                                                          1, 16)) {
    mvwaddstr(_valmonitor, 1, 1, "Val Monitor Test");
}

void ui::ValMonitor::refresh(void) {
    //Clear the screen
    wmove(_valmonitor, 0, 0);
    wclrtobot(_valmonitor);
    printVals();
    wrefresh(_valmonitor);
}

void ui::ValMonitor::printVals(void) {
    mvwaddstr(_valmonitor, 1, 1, "CTRL DATA");
    mvwaddstr(_valmonitor, 1, 15, "SENSOR DATA");
    mvwaddstr(_valmonitor, 1, 32, "LANE REC DATA");
    mvwaddstr(_valmonitor, 1, 52, "VEHICLE DATA");

    for(int i = 1; i < xsize; i++)
        mvwaddch(_valmonitor, 2, i, ACS_HLINE);

    //Current steering stuff
    mvwaddstr(_valmonitor, 3, 1, "Speed:");
    mvwprintw(_valmonitor, 3, 8, "%g", od::ConferenceData::instance()->getSpeed());

    mvwaddstr(_valmonitor, 4, 1, "Angle:");
    mvwprintw(_valmonitor, 4, 8, "%g", od::ConferenceData::instance()->getAngle());

    //Current sensor values
    mvwaddstr(_valmonitor, 3, 15, "US1:");
    mvwprintw(_valmonitor, 3, 20, "%g", od::ConferenceData::instance()->getUS1());

    mvwaddstr(_valmonitor, 4, 15, "US2:");
    mvwprintw(_valmonitor, 4, 20, "%g", od::ConferenceData::instance()->getUS2());

    mvwaddstr(_valmonitor, 5, 15, "IR1:");
    mvwprintw(_valmonitor, 5, 20, "%g", od::ConferenceData::instance()->getIR1());

    mvwaddstr(_valmonitor, 6, 15, "IR2:");
    mvwprintw(_valmonitor, 6, 20, "%g", od::ConferenceData::instance()->getIR2());

    mvwaddstr(_valmonitor, 7, 15, "IR3:");
    mvwprintw(_valmonitor, 7, 20, "%g", od::ConferenceData::instance()->getIR3());

    mvwaddstr(_valmonitor, 3, 32, "Angle:");
    mvwprintw(_valmonitor, 3, 44, "%g", od::ConferenceData::instance()->getRec_angle());

    mvwaddstr(_valmonitor, 4, 32, "Line dist.:");
    mvwprintw(_valmonitor, 4, 44, "%g", od::ConferenceData::instance()->getDistance_to_line());

    mvwaddstr(_valmonitor, 5, 32, "Quality:");
    if(od::ConferenceData::instance()->isQuality())
        mvwaddch(_valmonitor, 5, 44, '+');
    else
        mvwaddch(_valmonitor, 5, 44, '-');

    mvwaddstr(_valmonitor, 3, 52, "Abs TP:");
    mvwprintw(_valmonitor, 3, 60, "%g", od::ConferenceData::instance()->getAbsPath());
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