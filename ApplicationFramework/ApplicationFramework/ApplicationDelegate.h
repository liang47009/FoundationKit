#ifndef LOSEMYMIND_APPLICATIONDELEGATE_H
#define LOSEMYMIND_APPLICATIONDELEGATE_H


class ApplicationDelegate
{

public:

    /**
     * @brief   Application launching.
     * @return true    Initialize success, app continue.
     * @return false   Initialize failed, app terminate.
     */
    virtual void OnFinishLaunching() = 0;

    /**
     * @brief  This function will be called when the application enters background.
     */
    virtual void OnEnterBackground() = 0;

    /**
     * @brief  This function will be called when the application enters foreground.
     */
    virtual void OnEnterForeground() = 0;

    /**
     * @brief  This function will be called when the system low memory.
     */
    virtual void OnReceiveMemoryWarning() = 0;

    /**
     * @brief  This function will be called when the application will terminate.
     */
    virtual void OnWillTerminate() = 0;

};
#endif // LOSEMYMIND_APPLICATIONDELEGATE_H