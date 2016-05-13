/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Class to represent a ncurses window for showing and modifying the current
 * camera setup.
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
        int selectedItem;
        bool active;
        WINDOW* _camsettings;
        void printVals(void);
    };
}

#endif //ATCONFIGURATOR_CAMSETTINGS_H
