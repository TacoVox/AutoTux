//
// Created by jonas on 4/25/16.
//

#ifndef ATCONFIGURATOR_MAINUI_H
#define ATCONFIGURATOR_MAINUI_H

#include <string>
#include <ncurses.h>
#include <memory>
#include "ui/Header.h"
#include "ui/Footer.h"

namespace ui {
    class MainUI {
    public:
        MainUI(void);
        void showMainUI(void);
        void inputLoop(void);
        void quit(void);
    private:
        WINDOW* _window;
        int xsize;
        int ysize;
        bool interrupted;
        Header header;
        Footer footer;
        void init(void);
    };
}

#endif //ATCONFIGURATOR_MAINUI_H
