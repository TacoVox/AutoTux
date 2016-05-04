//
// Created by jonas on 4/27/16.
//

#ifndef ATCONFIGURATOR_ATCWINDOW_H
#define ATCONFIGURATOR_ATCWINDOW_H

namespace ui {
    class ATCWindow {
    public:
        virtual void refresh(void) = 0;
        virtual void select(void) = 0;
        virtual void unselect(void) = 0;
        virtual void selUp(void) = 0;
        virtual void selDn(void) = 0;
        virtual void selLeft(void) = 0;
        virtual void selRight(void) = 0;
        virtual void incr(void) = 0;
        virtual void decr(void) = 0;
    };
}

#endif //ATCONFIGURATOR_ATCWINDOW_H
