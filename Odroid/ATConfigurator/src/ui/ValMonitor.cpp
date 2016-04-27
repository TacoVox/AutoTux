//
// Created by jonas on 4/26/16.
//

#include "ui/ValMonitor.h"

ui::ValMonitor::ValMonitor(void) : xsize(78), ysize(38),
                                   _valmonitor(newwin(1, xsize, 1, 0)) {}

ui::ValMonitor::ValMonitor(int x, int y) : xsize(x - 2), ysize(y - 2),
                                       _valmonitor(newwin(1, xsize, 1, 0)) {
}

void ui::ValMonitor::refresh(void) {
    //Refresh the stuff here.
}