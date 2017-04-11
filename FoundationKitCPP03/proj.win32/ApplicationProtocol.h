#pragma once

class ApplicationProtocol
{
public:

    virtual ~ApplicationProtocol(){

    }

	/**
	 * App start 
	 */
	virtual void applicationDidLaunching() = 0;

    /**
    @brief    Implement engine init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching() = 0;

    /**
    @brief  This function will be called when the application enters background.

    */
    virtual void applicationDidEnterBackground() = 0;

    /**
    @brief  This function will be called when the application enters foreground.

    */
    virtual void applicationWillEnterForeground() = 0;

	virtual void applicationWillTerminate() = 0;

};

