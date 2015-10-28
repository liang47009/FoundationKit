#pragma once
#include "Application.h"

class  AppDelegate : private Application
{
public:
	AppDelegate();
	virtual ~AppDelegate();

	/**
	* App start
	*/
	virtual void applicationDidLaunching();

	/**
	@brief    Implement engine init code here.
	@return true    Initialize success, app continue.
	@return false   Initialize failed, app terminate.
	*/
	virtual bool applicationDidFinishLaunching();

	/**
	@brief  This function will be called when the application enters background.

	*/
	virtual void applicationDidEnterBackground();

	/**
	@brief  This function will be called when the application enters foreground.

	*/
	virtual void applicationWillEnterForeground();


	virtual void applicationWillTerminate();

	void mainLoop();

};


