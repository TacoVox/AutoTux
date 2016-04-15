//
// Created by Jonas Kahler on 4/1/16.
//
#include <iostream>
#include <thread>
#include <signal.h>
#include <proxy/Proxy.h>


using namespace std;

void exit_handler(int);

int32_t main(int32_t argc, char **argv) {

    signal(SIGINT, exit_handler);

    cout << "Starting up AutoTuxProxy..." << endl;

    proxy::Proxy p(argc, argv);

    return p.runModule();
}


void exit_handler(int num)
{
    cout << "caught signal: " << num << endl;
    exit(1);
}
