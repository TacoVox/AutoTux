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
#include "ui/Menu.h"

namespace ui {
    class MainUI {
    public:
        MainUI(void);
        void mainLoop(void);
        void inputLoop(void);
        void quit(void);
    private:
        short int mode;
        WINDOW* _window;
        int xsize;
        int ysize;
        bool interrupted;
        int keyPressed;
        Header header;
        Footer footer;
        Menu menu;
        void handleKeys(void);
    };
}

#endif //ATCONFIGURATOR_MAINUI_H
