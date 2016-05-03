//
// Created by jonas on 4/26/16.
//

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
        const char* dtostr(std::string, double);
    };
}

#endif //ATCONFIGURATOR_VALMONITOR_H
