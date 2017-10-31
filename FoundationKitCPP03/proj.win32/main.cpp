
#include "AppDelegate.h"
#include <iostream>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Foundation/Size.hpp"

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

void setWindowSize(HWND mainWindow, Size size)
{
    Size _winScreenSize = Size(GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
    SetWindowPos(mainWindow
        , NULL
        ,(_winScreenSize.Width - size.Width) / 2
        ,(_winScreenSize.Height - size.Height) / 2
        ,size.Width
        ,size.Height,SWP_NOZORDER|SWP_SHOWWINDOW);
    UpdateWindow(mainWindow);
}

LRESULT CALLBACK onMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch(uMsg)
    {
        case WM_DESTROY:
        PostQuitMessage(0);
        break;
        case WM_LBUTTONUP:
        {
            FKLog("============ WM_LBUTTONUP");
        }
        break;
        case WM_SIZE:
            {
                RECT rc;
                GetWindowRect(hWnd,&rc);
                //int iWidth = rc.right-rc.left;
                //int iHeight = rc.bottom-rc.top;
                //iWidth  += GetSystemMetrics(SM_CXDLGFRAME)*2;
                //iHeight += GetSystemMetrics(SM_CYDLGFRAME)*2;
                //iHeight +=GetSystemMetrics(SM_CYCAPTION);

                //FKLog("========== iWidth:%d, iHeight:%d ",iWidth, iHeight);

                switch(wParam)
                {
                case SIZE_MAXIMIZED:
                    FKLog("======= SIZE_MAXIMIZED width:%d, height%d", rc.right -rc.left, rc.bottom-rc.top);
                    break;
                case SIZE_MINIMIZED:
                    FKLog("======= SIZE_MINIMIZED width:%d, height%d", rc.right -rc.left, rc.bottom-rc.top);
                    break;
                case SIZE_RESTORED:
                    FKLog("======= SIZE_RESTORED width:%d, height%d", rc.right -rc.left, rc.bottom-rc.top);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
int APIENTRY wWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    std::string resourceRoot ="";

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

        Size winScreenSize = Size(GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
        winScreenSize.Width  += GetSystemMetrics(SM_CXDLGFRAME)*2;
        winScreenSize.Height += GetSystemMetrics(SM_CYDLGFRAME)*2;
        winScreenSize.Height +=GetSystemMetrics(SM_CYCAPTION);

        //Size windowSize = winScreenSize;
        Size windowSize(1280,720);
        int dwStyle = WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
        int fixedXPos = GetSystemMetrics(SM_CXDLGFRAME);
        int nCmdWindow = SW_SHOW;
        if (!bWindowed)
        {
            fixedXPos = 0;
            dwStyle = WS_POPUP;
            nCmdWindow = SW_MAXIMIZE;
        }

        WCHAR title[256] = { 0 };
        swprintf_s(title, wstrViewName.c_str());
        _mainWindow = CreateWindow(TEXT("fm_main"), title, dwStyle,
            (winScreenSize.Width- windowSize.Width) / 2-fixedXPos, 
            (winScreenSize.Height - windowSize.Height) / 2, 
            windowSize.Width, windowSize.Height, NULL, NULL, hInstance, NULL);

        if (_mainWindow == NULL)
        {
            DWORD dwError = 	GetLastError();
            FKLog("****** Create window failed:%d, %s", dwError, strerror(dwError));
            break;
        }
        ShowWindow(_mainWindow, nCmdWindow);
        UpdateWindow(_mainWindow);
        bSuccess = true;
    } while (false);

    MSG msg;
    AppDelegate app;
    Application::getInstance()->applicationDidLaunching();
    Application::getInstance()->applicationDidFinishLaunching();
    while (true)
    {
        if(PeekMessage(&msg,_mainWindow,0,0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

