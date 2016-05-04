//
// Created by Jonas Kahler on 4/27/16.
//

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
        void selDn(void);
        void selUp(void);
        void selLeft(void);
        void selRight(void);
        void select(void);
        void unselect(void);
        void incr(void);
        void decr(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _menu;
        std::vector<std::string> items;
        std::vector<std::unique_ptr<ui::ATCWindow>> windows;
        int curritem;
        int currwindow;
        bool menusel;
        void genMenu(void);
    };
}

#endif //ATCONFIGURATOR_MENU_H
