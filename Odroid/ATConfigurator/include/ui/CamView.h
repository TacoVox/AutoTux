/**
 * @author Jonas Kahler // jonas@derkahler.de
 * Class to represent a ncurses window for showing the current WebCam camera
 * image.
 */

#ifndef ATCONFIGURATOR_CAMVIEW_H
#define ATCONFIGURATOR_CAMVIEW_H

#include <ncurses.h>
#include <string>
#include "ui/ATCWindow.h"

namespace ui {
    class CamView : public ATCWindow {
    public:
        CamView(void);
        CamView(int, int);
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
        WINDOW* _camview;
        std::string image;
        std::string loadImage(void);
    };
}

#endif //ATCONFIGURATOR_CAMVIEW_H
