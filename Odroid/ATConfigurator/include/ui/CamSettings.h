//
// Created by jonas on 5/3/16.
//

#ifndef ATCONFIGURATOR_CAMSETTINGS_H
#define ATCONFIGURATOR_CAMSETTINGS_H

#include "ui/ATCWindow.h"
#include <ncurses.h>
#include <string>

namespace ui {
    class CamSettings : public ATCWindow {
    public:
        CamSettings(void);
        CamSettings(int, int);
        void refresh(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _camsettings;
        void printVals(void);
        const char* dtostr(std::string, double);
    };
}

#endif //ATCONFIGURATOR_CAMSETTINGS_H
