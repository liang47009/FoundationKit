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
void createCommand()
{
    if (bCreateCmd)
    {
        return;
    }
    bCreateCmd = true;
    std::thread th([&](){
        int nCmd = -1;
        std::cin >> nCmd;
        command = nCmd;
        bCreateCmd = false;
    });
    th.detach();
}

int wmain()
{
    std::error_code ec;
    std::string strError = ec.message();

    AppDelegate app;
    Application::getInstance()->applicationDidLaunching();
    Application::getInstance()->applicationDidFinishLaunching();
    while (true)
    {
        createCommand();
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
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    Application::getInstance()->applicationWillTerminate();
    return 0;
}


