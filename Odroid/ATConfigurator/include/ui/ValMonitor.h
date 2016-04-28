//
// Created by jonas on 4/26/16.
//

#ifndef ATCONFIGURATOR_VALMONITOR_H
#define ATCONFIGURATOR_VALMONITOR_H

#include "ui/ATCWindow.h"
#include <ncurses.h>

namespace ui {
    class ValMonitor : public ATCWindow {
    public:
        ValMonitor(void);
        ValMonitor(int, int);
        void refresh(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _valmonitor;
    };
}

#endif //ATCONFIGURATOR_VALMONITOR_H