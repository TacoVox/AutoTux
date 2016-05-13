/**
 * @author Jonas Kahler // jonas@derkahler.de
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
