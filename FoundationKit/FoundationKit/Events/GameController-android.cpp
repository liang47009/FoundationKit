

#include "GameController.h"

#if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)

#include <functional>
#include <algorithm>
#include "FoundationKit/Events/EventController.h"
#include "FoundationKit/Platform/Android/AndroidJavaObject.h"
#include "FoundationKit/Platform/Android/AndroidJNIHelper.h"


NS_FK_BEGIN

using namespace FoundationKit::Android;

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller)
            : _controller(controller)
    {
    }

    static std::vector<Controller*>::iterator findController(const std::string& deviceName, int deviceId)
    {
        auto iter = std::find_if(Controller::s_allController.begin(), Controller::s_allController.end(), [&](Controller* controller){
                return (deviceName == controller->_deviceName) && (deviceId == controller->_deviceId);
            });

        return iter;
    }

    static void onConnected(const std::string& deviceName, int deviceId)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter != Controller::s_allController.end())
            return;

        // It's a new controller being connected.
        auto controller = new FoundationKit::Controller();
        controller->_deviceId = deviceId;
        controller->_deviceName = deviceName;
        Controller::s_allController.push_back(controller);

        controller->onConnected();
    }

    static void onDisconnected(const std::string& deviceName, int deviceId)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            return;
        }

        (*iter)->onDisconnected();
        Controller::s_allController.erase(iter);
    }

    static void onButtonEvent(const std::string& deviceName, int deviceId, int keyCode, bool isPressed, float value, bool isAnalog)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            onConnected(deviceName, deviceId);
            iter = findController(deviceName, deviceId);
        }

        (*iter)->onButtonEvent(keyCode, isPressed, value, isAnalog);
    }

    static void onAxisEvent(const std::string& deviceName, int deviceId, int axisCode, float value, bool isAnalog)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            onConnected(deviceName, deviceId);
            iter = findController(deviceName, deviceId);
        }
        
        (*iter)->onAxisEvent(axisCode, value, isAnalog);
    }

private:
    Controller* _controller;
};

void Controller::startDiscoveryController()
{
    // Empty implementation on Android
}

void Controller::stopDiscoveryController()
{
    // Empty implementation on Android
}

Controller::~Controller()
{
    delete _impl;

    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners()
{
}

bool Controller::isConnected() const
{
    // If there is a controller instance, it means that the controller is connected.
    // If a controller is disconnected, the instance will be destroyed.
    // So always returns true for this method.
    return true;
}

Controller::Controller()
    : _controllerTag(TAG_UNSET)
    , _impl(new ControllerImpl(this))
    , _connectEvent(nullptr)
    , _keyEvent(nullptr)
    , _axisEvent(nullptr)
{
    init();
}

void Controller::receiveExternalKeyEvent(int externalKeyCode,bool receive)
{
    JniMethodInfo t;
    if (AndroidJNIHelper::getInstance()->getStaticMethodInfo(t, 
        "com/gamecontroller/lib/GameControllerHelper", 
        "receiveExternalKeyEvent", 
        "(IIZ)V")) {

        t.env->CallStaticVoidMethod(t.classID, t.methodID, _deviceId, externalKeyCode, receive);
        t.env->DeleteLocalRef(t.classID);
    }
}

NS_FK_END

extern "C" {

    void Java_com_gamecontroller_lib_GameControllerAdapter_nativeControllerConnected(JNIEnv*  env, jobject thiz, jstring deviceName, jint controllerID)
    {
        FoundationKit::ControllerImpl::onConnected(JNIHELPER->jstring2string(deviceName), controllerID);
    }

    void Java_com_gamecontroller_lib_GameControllerAdapter_nativeControllerDisconnected(JNIEnv*  env, jobject thiz, jstring deviceName, jint controllerID)
    {
        FoundationKit::ControllerImpl::onDisconnected(JNIHELPER->jstring2string(deviceName), controllerID);
    }

    void Java_com_gamecontroller_lib_GameControllerAdapter_nativeControllerButtonEvent(JNIEnv*  env, jobject thiz, jstring deviceName, jint controllerID, jint button, jboolean isPressed, jfloat value, jboolean isAnalog)
    {
        FoundationKit::ControllerImpl::onButtonEvent(JNIHELPER->jstring2string(deviceName), controllerID, button, isPressed, value, isAnalog);
    }

    void Java_com_gamecontroller_lib_GameControllerAdapter_nativeControllerAxisEvent(JNIEnv*  env, jobject thiz, jstring deviceName, jint controllerID, jint axis, jfloat value, jboolean isAnalog)
    {
        FoundationKit::ControllerImpl::onAxisEvent(JNIHELPER->jstring2string(deviceName), controllerID, axis, value, isAnalog);
    }

} // extern "C" {


#endif // #if (FK_TARGET_PLATFORM == FK_PLATFORM_ANDROID)
