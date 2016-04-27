#include <iostream>
#include <thread>
#include "ui/MainUI.h"

using namespace std;

//Include OD Later!!!

int main() {
    shared_ptr<ui::MainUI> mainUI = (shared_ptr<ui::MainUI>) new ui::MainUI();
    mainUI->showMainUI();
    thread uithread(&ui::MainUI::inputLoop, mainUI);
    uithread.join();

    return 0;
}