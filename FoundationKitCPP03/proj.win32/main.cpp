
#include "AppDelegate.h"
#include <iostream>
#include "FoundationKit/Foundation/Timer.hpp"
#include "FoundationKit/Foundation/Logger.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/FileUtils.hpp"

USING_NS_FK;

bool bCanExit = false;
static std::atomic<int> command;
static std::atomic<bool> bCreateCmd;

void createWindows();


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

LRESULT CALLBACK onMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
int APIENTRY wWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::string resourceRoot =FileUtils::getInstance()->getResourceRootPath();

    bool bWindowed = true;
    std::string appIconPath = resourceRoot+"..\\res\\app.ico";
    std::string cursorPath = resourceRoot+"..\\res\\arrow.ICO";

    std::string viewName = "我的窗口";
    HWND     _mainWindow;
    bool bSuccess = false;
    do 
    {
        std::wstring wstrViewName = StringUtils::string2UTF8wstring(viewName);
        // 创建Windows窗口
        WNDCLASSEX wndclass;
        wndclass.cbSize = sizeof(wndclass);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = onMsgProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;//sizeof(WORD);
        wndclass.hInstance = hInstance;
        if (appIconPath.empty())
        {
            wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
            wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        }
        else
        {
            std::wstring iconPath = StringUtils::string2UTF8wstring(appIconPath);
            wndclass.hIcon = (HICON)LoadImageW(hInstance, iconPath.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
            wndclass.hIconSm = (HICON)LoadImageW(hInstance, iconPath.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
        }
        if (cursorPath.empty())
        {
            wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
        }
        else
        {
            std::wstring iconPath = StringUtils::string2UTF8wstring(cursorPath);
            wndclass.hCursor =(HCURSOR)LoadImageW(hInstance, iconPath.c_str(),IMAGE_CURSOR,0,0,LR_LOADFROMFILE);
        }
        wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = TEXT("fm_main");


        RegisterClassEx(&wndclass);

        RECT winRect;
        SetRect(&winRect,0,0,600,320);
        AdjustWindowRectEx(&winRect, WS_CAPTION|WS_SYSMENU, false, 0);

        int x =  -winRect.left;
        winRect.left += x;
        winRect.right += x;
        int y =  -winRect.top;
        winRect.top += y;
        winRect.bottom += y;

        int dwStyle = WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
        int nCmdWindow = SW_SHOW;
        if (!bWindowed)
        {
            dwStyle = WS_POPUP;
            nCmdWindow = SW_MAXIMIZE;
        }

        WCHAR title[256] = { 0 };
        swprintf_s(title, wstrViewName.c_str());
        _mainWindow = CreateWindow(TEXT("fm_main"), title, dwStyle,
            (GetSystemMetrics(SM_CXSCREEN) - 600) / 2, 
            (GetSystemMetrics(SM_CYSCREEN) - 320) / 2, 
            winRect.right-winRect.left, winRect.bottom-winRect.top, NULL, NULL, hInstance, NULL);

        if (_mainWindow == NULL)
        {
            DWORD dwError = 	GetLastError();
            LOG_ERROR("****** Create window failed:%d, %s", dwError, strerror(dwError));
            break;
        }
        ShowWindow(_mainWindow, nCmdWindow);
        UpdateWindow(_mainWindow);
        bSuccess = true;
    } while (false);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

