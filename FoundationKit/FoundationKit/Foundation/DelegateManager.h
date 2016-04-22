
/****************************************************************************
 Copyright (c) 2016 libo All rights reserved.
 
 losemymind.libo@gmail.com
 
 ****************************************************************************/

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Foundation/Singleton.h"

NS_FK_BEGIN

typedef std::function<void(const void*)>   DelegateCallbackType;
class DelegateObserver
{
public:
    typedef std::shared_ptr<DelegateObserver> Pointer;

   /** @brief 创建一个观察者
    *  @param[in] name     delegate的名称，一个名称可以对应多个观察者
    *  @param[in] selector DelegateObserver回调的函数。
    *  @param[in] target   回调函数所在的对象，如果是全局函数，target为nullptr，一个对象可以有多个观察者。
    *  @param[in] callOnce 如果这个值是true，那么这个观察者将会在回调函数调用后删除。
    */

    static Pointer create(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);

    ~DelegateObserver();

   /** @brief 调用这个观察者的回调函数。
    *  @param[in] args    The param.
    */
    void                          invoke(const void* args);
    void*                         getTarget()const;
    const DelegateCallbackType&   getSelector()const;
    const std::string&            getName()const;
    bool                          isCallOnce();

    bool operator ==(const DelegateObserver& other);
    bool operator != (const DelegateObserver& other);
protected:
   /** @brief DelegateObserver 构造函数
    *  @param[in] name     delegate的名称，一个名称可以对应多个观察者
    *  @param[in] selector DelegateObserver回调的函数。
    *  @param[in] target   回调函数所在的对象，如果是全局函数，target为nullptr，一个对象可以有多个观察者。
    *  @param[in] callOnce 如果这个值是true，那么这个观察者将会在回调函数调用后删除。
    */
    DelegateObserver(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);
private:
    std::string             _name;
    DelegateCallbackType    _selector;
    void*                   _target;
    bool                    _callOnce;
};

inline bool operator==(const DelegateObserver& l, const DelegateObserver& r)
{
    return (l.getTarget() == r.getTarget() && l.getName() == r.getName());
}

inline bool operator!=(const DelegateObserver& l, const DelegateObserver& r)
{
    return !(l == r);
}

class DelegateManager : public Singleton<DelegateManager>
{
    friend Singleton<DelegateManager>;
    typedef std::vector<DelegateObserver::Pointer>   ObserverList;


public:
    ~DelegateManager();
   /** @brief 添加一个观察者
    *  @param[in] name     delegate的名称，一个名称可以对应多个观察者
    *  @param[in] selector DelegateObserver回调的函数。
    *  @param[in] target   回调函数所在的对象，如果是全局函数，target为nullptr，一个对象可以有多个观察者。
    *  @param[in] callOnce 如果这个值是true，那么这个观察者将会在回调函数调用后删除。
    */
    void addObserver(const std::string& name, DelegateCallbackType& selector, void* target = nullptr, bool callOnce = false);



   /** @brief 移除观察者
    *  @param[in] name     delegate的名称，这个名称下面的所有观察者都将被移除。
    */
    void removeObserver( const std::string& name);
    void removeObserver( const char* name);

   /** @brief 移除观察者
    *  @param[in] target     回调函数所在的对象，如果是全局函数，target为nullptr，这个target下面的所有
    *                        观察者都将被移除。
    */
    void removeObserver( void* target);

   /** @brief 移除观察者
    *  @param[in] name     delegate的名称，
    *  @param[in] target   回调函数所在的对象，如果是全局函数，target为nullptr
    *                      如果一个观察者的name和target与指定的相同，那么
    *                      这个观察者将被移除。
    *                       
    */
    void removeObserver(const std::string& name, void* target);

   /** @brief 调用观察者
    *  @param[in] name     delegate的名称，
    *  @param[in] args     回调函数需要的参数
    *                       
    */
    void invokeDelegate(const std::string& name, const void* args);

protected:
    // Check whether the observer exists by the specified target and name.
    bool observerExisted(const std::string& name,void* target = nullptr);

private:
    DelegateManager();
    ObserverList   _observers;
    std::mutex     _mutex;
};



NS_FK_END












