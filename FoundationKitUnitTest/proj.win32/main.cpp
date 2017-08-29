#include "tracey.hpp"

#include "AppDelegate.h"
#include <thread>
#include <iostream>
#include <atomic>
#include "FoundationKit/Foundation/Timer.hpp"

USING_NS_FK;

bool bCanExit = false;
static std::atomic<int> command = -1;
static std::atomic<bool> bCreateCmd = false;



int wmain()
{
    std::error_code ec;
    std::string strError = ec.message();

    std::thread th([&]() {
        do 
        {
            int nCmd = -1;
            std::cin >> nCmd;
            command = nCmd;
        } while (command != 0);
    });

    AppDelegate app;
    Application::getInstance()->applicationDidLaunching();
    Application::getInstance()->applicationDidFinishLaunching();
    while (true)
    {
        switch (command)
        {
        case 0:
            bCanExit = true;
            break;
        case 1:
            break;
        default:
            break;
        }
        command = -1;
        Application::getInstance()->mainLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        if (bCanExit)
        {
            break;
        }
    }
    Application::getInstance()->applicationWillTerminate();

    if (th.joinable())
    {
        th.join();
    }

    return 0;
}


