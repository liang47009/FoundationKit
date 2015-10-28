#ifndef FoundationKit_IMEDelegate_H
#define FoundationKit_IMEDelegate_H


#include <string>
#include "FoundationKit/Math/Geometry.h"

NS_FK_BEGIN

/**
 * A static global empty std::string install.
 */
extern const std::string STD_STRING_EMPTY;


/**
 * Keyboard notification event type.
 */
typedef struct
{
    Rect  begin;              // the soft keyboard rectangle when animation begins
    Rect  end;                // the soft keyboard rectangle when animation ends
    float     duration;           // the soft keyboard animation duration
} IMEKeyboardNotificationInfo;

/**
 *@brief    Input method editor delegate.
 */
class IMEDelegate
{
public:
    /**
     * Default constructor.
     */
    virtual ~IMEDelegate();
    
    /**
     * Default destructor.
     */
    virtual bool attachWithIME();
    
    /**
     * Determine whether the IME is detached or not.
     */
    virtual bool detachWithIME();

protected:
    friend class IMEDispatcher;

    /**
    @brief    Decide if the delegate instance is ready to receive an IME message.

    Called by IMEDispatcher.
    */
    virtual bool canAttachWithIME() { return false; }
    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didAttachWithIME() {}

    /**
    @brief    Decide if the delegate instance can stop receiving IME messages.
    */
    virtual bool canDetachWithIME() { return false; }

    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    */
    virtual void didDetachWithIME() {}

    /**
    @brief    Called by IMEDispatcher when text input received from the IME.
    */
    virtual void insertText(const char * text, size_t len) {UNUSED_PARAM(text);UNUSED_PARAM(len);}

    /**
    @brief    Called by IMEDispatcher after the user clicks the backward key.
    */
    virtual void deleteBackward() {}

    /**
    @brief    Called by IMEDispatcher for text stored in delegate.
    */
    virtual const std::string& getContentText() { return STD_STRING_EMPTY; }

    //////////////////////////////////////////////////////////////////////////
    // keyboard show/hide notification
    //////////////////////////////////////////////////////////////////////////
    /**
     */
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo& info)   { UNUSED_PARAM(info); }

    virtual void keyboardDidShow(IMEKeyboardNotificationInfo& info)    { UNUSED_PARAM(info); }

    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& info)   { UNUSED_PARAM(info); }

    virtual void keyboardDidHide(IMEKeyboardNotificationInfo& info)    { UNUSED_PARAM(info); }

protected:
    IMEDelegate();
};


NS_FK_END
#endif // FoundationKit_IMEDelegate_H

