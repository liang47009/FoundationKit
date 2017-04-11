
#include "AppDelegate.h"
#include <iostream>
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Logger.hpp"

USING_NS_FK;

bool bCanExit = false;
static std::atomic<int> command;
static std::atomic<bool> bCreateCmd;

int wmain()
{
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

        if (bCanExit)
        {
            break;
        }
        
        Application::getInstance()->mainLoop();
        std::this_thread::sleep_for(std::chrono::milliseconds((1000 / 60)));
    }
    Application::getInstance()->applicationWillTerminate();
    return 0;
}


