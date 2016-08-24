#ifndef FoundationKit_IMEDispatcher_H
#define FoundationKit_IMEDispatcher_H


#include "IMEDelegate.h"
#include "FoundationKit/Foundation/Singleton.h"

NS_FK_BEGIN


/**
@brief    Input Method Edit Message Dispatcher.
*/
class IMEDispatcher :public Singleton<IMEDispatcher>
{
public:

    friend class Singleton < IMEDispatcher > ;
    ~IMEDispatcher();

    /**
     * @brief Dispatches the input text from IME.
     */
    void dispatchInsertText(const char * text, size_t len);

    /**
     * @brief Dispatches the delete-backward operation.
     */
    void dispatchDeleteBackward();

    /**
     * @brief Get the content text from IMEDelegate, retrieved previously from IME.
     */
    const std::string& getContentText();

    //////////////////////////////////////////////////////////////////////////
    // dispatch keyboard notification
    //////////////////////////////////////////////////////////////////////////
    /**
     */
    void dispatchKeyboardWillShow(IMEKeyboardNotificationInfo& info);

    void dispatchKeyboardDidShow(IMEKeyboardNotificationInfo& info);

    void dispatchKeyboardWillHide(IMEKeyboardNotificationInfo& info);

    void dispatchKeyboardDidHide(IMEKeyboardNotificationInfo& info);

protected:
    friend class IMEDelegate;

    /**
     *@brief Add delegate to receive IME messages.
     *@param delegate A instance implements IMEDelegate delegate.
     */
    void addDelegate(IMEDelegate * delegate);

    /**
     *@brief Attach the Delegate to the IME.
     *@param delegate A instance implements IMEDelegate delegate.
     *@return If the old delegate can detach from the IME, and the new delegate 
     *       can attach to the IME, return true, otherwise false.
     */
    bool attachDelegateWithIME(IMEDelegate * delegate);

    /**
     * Dettach the delegate to the IME
     *@see `attachDelegateWithIME(IMEDelegate*)`
     *@param delegate  A instance implements IMEDelegate delegate.
     *@return Whether the IME is detached or not.
     */
    bool detachDelegateWithIME(IMEDelegate * delegate);

    /**
     *@brief Remove the delegate from the delegates which receive IME messages.
     *@param delegate A instance implements the IMEDelegate delegate.
     */
    void removeDelegate(IMEDelegate * delegate);

private:
    IMEDispatcher();
    
    class Impl;
    Impl * _impl;
};


NS_FK_END

#endif // FoundationKit_IMEDispatcher_H
