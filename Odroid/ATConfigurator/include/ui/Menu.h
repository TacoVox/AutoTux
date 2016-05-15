/**
 * @author Jonas Kahler // jonas@derkahler.de
 */

#ifndef ATCONFIGURATOR_MENU_H
#define ATCONFIGURATOR_MENU_H

#include "ui/ATCWindow.h"
#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>

namespace ui {
    class Menu {
    public:
        Menu(void);
        Menu(int, int);
        void refresh(void);
        void setActive(bool);
        bool isActive(void);
        void selDn(void);
        void selUp(void);
        int getCurrent(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _menu;
        std::vector<std::string> items;
        int curritem;
        bool active;
        void genMenu(void);
    };
}

#endif //ATCONFIGURATOR_MENU_H
