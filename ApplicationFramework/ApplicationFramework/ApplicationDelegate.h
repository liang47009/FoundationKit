

class Application;

enum class EWindowMode
{
    /** The window is in true fullscreen mode */
    Fullscreen,
    /** The window has no border and takes up the entire area of the screen */
    WindowedFullscreen,
    /** The window has a border and may not take up the entire screen area */
    Windowed,
    /** Pseudo-fullscreen mode for devices like HMDs */
    WindowedMirror,

    /** The total number of supported window modes */
    NumWindowModes
};


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


    virtual void Update(float deltaTime) = 0;

    virtual void SetFrameInterval(float interval) = 0;

    virtual void SetWindowMode(EWindowMode mode) = 0;

};