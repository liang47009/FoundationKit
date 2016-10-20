
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

class Application
{
public:
    virtual void Update(float deltaTime) = 0;

    virtual void SetFrameInterval(float interval) = 0;

    virtual void SetWindowMode(EWindowMode mode) = 0;
};

