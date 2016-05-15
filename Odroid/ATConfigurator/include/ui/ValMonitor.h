/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Class to represent a ncurses window for showing the current values of the
 * sessions (like SensorBoard data, etc).
 */

#ifndef ATCONFIGURATOR_VALMONITOR_H
#define ATCONFIGURATOR_VALMONITOR_H

#include "ui/ATCWindow.h"
#include <ncurses.h>
#include <string>

namespace ui {
    class ValMonitor : public ATCWindow {
    public:
        ValMonitor(void);
        ValMonitor(int, int);
        // Overriding the virtual methods
        void refresh(void);
        void select(void);
        void unselect(void);
        void selUp(void);
        void selDn(void);
        void selLeft(void);
        void selRight(void);
        void incr(void);
        void decr(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _valmonitor;
        void printVals(void);
    };
}

#endif //ATCONFIGURATOR_VALMONITOR_H
