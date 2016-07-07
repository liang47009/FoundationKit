

#include "ApplicationDelegate.h"

class Application : public ApplicationDelegate
{
    Application();
public:

    virtual~Application();
    /**
     * @brief   Application launching.
     * @return true    Initialize success, app continue.
     * @return false   Initialize failed, app terminate.
     */
    virtual void OnFinishLaunching()override;

    /**
     * @brief  This function will be called when the application enters background.
     */
    virtual void OnEnterBackground()override;

    /**
     * @brief  This function will be called when the application enters foreground.
     */
    virtual void OnEnterForeground()override;

    /**
     * @brief  This function will be called when the system low memory.
     */
    virtual void OnReceiveMemoryWarning()override;

    /**
     * @brief  This function will be called when the application will terminate.
     */
    virtual void OnWillTerminate()override;


    virtual void Update(float deltaTime)override;

    virtual void SetFrameInterval(float interval)override;

    virtual void SetWindowMode(EWindowMode mode) override;
};