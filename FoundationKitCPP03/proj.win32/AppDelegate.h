#pragma once
#include "Application.h"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Foundation/FunctionHandler.hpp"
USING_NS_FK;

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
    DEPRECATED(1.0.2, "This is Test applicationDidFinishLaunching deprecated.")
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

    void TestTupleArgs(const ArgumentList& args);

};




