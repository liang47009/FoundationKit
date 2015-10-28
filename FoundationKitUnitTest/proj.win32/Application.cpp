
#include "Application.h"
#include <assert.h>


// sharedApplication pointer
Application * Application::sm_pSharedApplication = 0;


////////////////////////////////////////////////////////////////////////////////
// implement Application
////////////////////////////////////////////////////////////////////////////////

// sharedApplication pointer
Application * s_pSharedApplication = 0;

Application::Application()
{

    assert(! sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
	assert(this == sm_pSharedApplication);
    sm_pSharedApplication = NULL;
}

int Application::run()
{
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }
	return 0;
}


void Application::mainLoop()
{

}


//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    assert(sm_pSharedApplication);
    return sm_pSharedApplication;
}
