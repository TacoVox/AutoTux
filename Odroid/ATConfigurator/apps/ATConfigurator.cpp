/**
 * Main executable for the AutoTux ncurses configuration tool.
 * @author Jonas Kahler // jonas@derkahler.de
 */
#include <iostream>
#include <thread>
#include "ui/MainUI.h"
#include "od/ConferenceModule.h"

using namespace std;

int main(int32_t argc, char **argv) {
    //Create a pointer to a new main UI objects
    shared_ptr<ui::MainUI> mainUI = (shared_ptr<ui::MainUI>) new ui::MainUI();
    //Create a thread for refreshing the ui
    thread uithread(&ui::MainUI::mainLoop, mainUI);

    //Create a TimeTriggered Conference Client Module and start it
    od::ConferenceModule conferenceModule(argc, argv);
    conferenceModule.runModule();

    //Wait for termination of the thread
    uithread.join();

    return 0;
}