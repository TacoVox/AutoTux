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
        void selDown(void);
        void selUp(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _menu;
        std::vector<std::string> items;
        std::vector<std::unique_ptr<ui::ATCWindow>> windows;
        int curritem;
        void genMenu(void);
    };
}

#endif //ATCONFIGURATOR_MENU_H
