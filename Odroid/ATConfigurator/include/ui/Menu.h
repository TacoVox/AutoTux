//
// Created by Jonas Kahler on 4/27/16.
//

#ifndef ATCONFIGURATOR_MENU_H
#define ATCONFIGURATOR_MENU_H

#include <ncurses.h>
#include <string>
#include <vector>

namespace ui {
    class Menu {
    public:
        Menu(void);
        Menu(int, int);
        void refresh(void);
        void genMenu(void);
        void selDown(void);
        void selUp(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _menu;
        std::vector<std::string> items;
        int curritem;
    };
}

#endif //ATCONFIGURATOR_MENU_H
