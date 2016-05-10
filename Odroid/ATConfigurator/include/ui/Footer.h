//
// Created by jonas on 4/26/16.
//

#ifndef ATCONFIGURATOR_FOOTER_H
#define ATCONFIGURATOR_FOOTER_H

#include <ncurses.h>
#include <memory>
#include <ctime>

namespace ui {
    class Footer {
    public:
        Footer(void);
        Footer(int, int);
        void refresh(void);
    private:
        int xsize;
        int ysize;
        WINDOW* _footer;
        std::time_t time;
    };
}

#endif //ATCONFIGURATOR_FOOTER_H
