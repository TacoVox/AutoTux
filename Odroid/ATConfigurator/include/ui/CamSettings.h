/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

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
        int selectedItem;
        bool active;
        WINDOW* _camsettings;
        void printVals(void);
    };
}

#endif //ATCONFIGURATOR_CAMSETTINGS_H
