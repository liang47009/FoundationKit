#pragma once

#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN

// List of default axes to query for each controller
// Ideal solution is to call out to Java and enumerate the list of axes.
static const int32 AxisList[] =
{
    AMOTION_EVENT_AXIS_X,
    AMOTION_EVENT_AXIS_Y,
    AMOTION_EVENT_AXIS_Z,
    AMOTION_EVENT_AXIS_RX,
    AMOTION_EVENT_AXIS_RY,
    AMOTION_EVENT_AXIS_RZ,

    //These are DPAD analogs
    AMOTION_EVENT_AXIS_HAT_X,
    AMOTION_EVENT_AXIS_HAT_Y,
};

// List of desired gamepad keycodes
static const uint32 GamepadKeyList[] =
{
    AKEYCODE_BUTTON_A,
    AKEYCODE_DPAD_CENTER,
    AKEYCODE_BUTTON_B,
    AKEYCODE_BUTTON_X,
    AKEYCODE_BUTTON_Y,
    AKEYCODE_BUTTON_L1,
    AKEYCODE_BUTTON_R1,
    AKEYCODE_BUTTON_START,
    AKEYCODE_MENU,
    AKEYCODE_BUTTON_SELECT,
    AKEYCODE_BACK,
    AKEYCODE_BUTTON_THUMBL,
    AKEYCODE_BUTTON_THUMBR,
    AKEYCODE_BUTTON_L2,
    AKEYCODE_BUTTON_R2,
    AKEYCODE_DPAD_UP,
    AKEYCODE_DPAD_DOWN,
    AKEYCODE_DPAD_LEFT,
    AKEYCODE_DPAD_RIGHT
};


class Gamepad
{


};




NS_FK_END


