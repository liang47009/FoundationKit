#pragma once

#include "ApplicationProtocol.h"
#include <string>

struct WindowSize
{
	WindowSize(float w, float h)
		: width(w)
		, height(h)
	{}
	WindowSize()
		:width(0)
		, height(0)
	{}
	float  width;
	float  height;
};

class Application : public ApplicationProtocol
{
public:
	Application();
	virtual ~Application();

	/**
	@brief    Run the message loop.
	*/
	int run();

	virtual void mainLoop();

	void setWindowsSize(WindowSize size){ m_windowsize = size; }
	WindowSize getWindowSize(){ return m_windowsize; }

	/**
	@brief    Get current applicaiton instance.
	@return Current application instance pointer.
	*/
	static Application* getInstance();


protected:

	static Application * sm_pSharedApplication;

	WindowSize  m_windowsize;

};



