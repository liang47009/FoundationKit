/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_FOUNDATIONKIT_THREAD_SAFE_STACK_HPP
#define FOUNDATIONKIT_FOUNDATIONKIT_THREAD_SAFE_STACK_HPP

#include <atomic>
#include <memory>
#include <mutex>

template <typename _Ty>
class atomic_stack
{

public:
    typedef atomic_stack<_Ty> _Myt;
    typedef typename _Ty value_type;
    typedef typename std::shared_ptr<value_type> value_pointer;

protected:
    struct _Node
    {
        _Node(const value_type& val) : data(std::make_shared<value_type>(val)) {}
        _Node(const value_pointer& val) : data(val) {}
        value_pointer          data;
        std::shared_ptr<_Node> next;
        std::atomic<size_t> size{ 1 };  // container size
    };

    std::shared_ptr<_Node> _MyHead;  // should be atomic


    void _push(const std::shared_ptr<_Node>& head)
    {
        // Point head's next pointer at _MyHead
        head->next = std::atomic_load(&_MyHead);
        // Update head to new node
        do
        {
            if (head->next)
            {
                head->size = head->next->size + 1;
            }
        } while (!std::atomic_compare_exchange_weak(&_MyHead, &head->next, head));
    }

public:
    atomic_stack() = default;
    atomic_stack(const atomic_stack&) = delete;
    atomic_stack& operator=(const atomic_stack&) = delete;

    ~atomic_stack()
    {
        clear();
    }

    void push(const value_type& val)
    {
        // Make a new node
        _push(std::make_shared<_Node>(val));

    }

    void push(value_type&& val)
    {
        // Make a new node
        auto head = std::make_shared<_Node>(std::forward<value_type>(val));
        _push(head);
    }

    void push(const value_pointer& val)
    {
        // Make a new node
        _push(std::make_shared<_Node>(val));
    }

    template <typename... Args>
    void emplace(Args&&... args)
    {
        push(std::move(value_type(args...)));
    }


    template <typename _Ty2, typename... Args, typename = typename std::enable_if<std::is_base_of<_Ty>::value, _Ty2>::type>
    void emplace(Args&&... args)
    {
        push(std::move(_Ty2(args...)));
    }

    std::shared_ptr<value_type> pop()
    {
        std::shared_ptr<_Node> old_head = std::atomic_load(&_MyHead);
        while (old_head && !std::atomic_compare_exchange_weak(&_MyHead, &old_head, old_head->next)) {}
        return old_head ? old_head->data : std::shared_ptr<value_type>();

    }

    bool empty() const
    {
        return std::atomic_load(&_MyHead) == nullptr;
    }

    size_t size() const
    {
        auto head = std::atomic_load(&_MyHead);
        if (head)
        {
            return head->size;
        }
        else
        {
            return 0;
        }
    }

    void swap(_Myt& other) noexcept
    {
        auto head = std::atomic_load(&_MyHead);
        auto result = std::atomic_exchange(&other._MyHead, head);
        std::atomic_store(&_MyHead, head);
    }

    void clear()
    {
        std::shared_ptr<_Node> head = std::atomic_load(&_MyHead);
        while (head)
        {
            while (!std::atomic_compare_exchange_weak(&_MyHead, &head, head->next)) {}
            head = std::atomic_load(&_MyHead);
        }
    }
};


template <typename _Ty>
class mutex_stack
{

public:
    typedef atomic_stack<_Ty> _Myt;
    typedef typename _Ty value_type;
    typedef typename std::shared_ptr<value_type> value_pointer;
protected:
    struct _Node
    {
        _Node(const value_type& val) : data(std::make_shared<value_type>(val)) {}
        std::shared_ptr<value_type> data;
        std::shared_ptr<_Node> next;
        std::atomic<size_t> size{ 1 };  // container size
    };

    std::shared_ptr<_Node> _MyHead;  // should be atomic
    std::mutex             _mutex;
public:
    mutex_stack() = default;
    mutex_stack(const mutex_stack&) = delete;
    mutex_stack& operator=(const mutex_stack&) = delete;

    ~mutex_stack()
    {
        clear();
    }

    void push(const value_type& val)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        // Make a new node
        auto head = std::make_shared<_Node>(val);
        // Point p's next pointer at head
        head->next = _MyHead;
    }

    void push(value_type&& val)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        // Make a new node
        auto head = std::make_shared<_Node>(std::forward<value_type>(val));
        // Point p's next pointer at head
        head->next = _MyHead;
    }

    void push(const value_pointer& val)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        // Make a new node
        auto head = std::make_shared<_Node>(val);
        // Point p's next pointer at head
        head->next = _MyHead;
    }

    template <class... Args>
    void emplace(Args&&... args)
    {
        push(std::move(value_type(args...)));
    }
    template <typename _Ty2, typename... Args, typename = typename std::enable_if<std::is_base_of<_Ty>::value, _Ty2>::type>
    void emplace(Args&&... args)
    {
        push(std::move(_Ty2(args...)));
    }

    std::shared_ptr<value_type> pop()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::shared_ptr<_Node> old_head = _MyHead;
        _MyHead = old_head->next;
        return old_head;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _MyHead == nullptr;
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_MyHead)
        {
            return _MyHead->size;
        }
        else
        {
            return 0;
        }
    }

    void swap(_Myt& other) noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto temp = _MyHead;
        _MyHead = other._MyHead;
        other._MyHead = temp;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::shared_ptr<_Node> head = _MyHead;
        while (head)
        {
            _MyHead = head->next;
            head = nullptr;
            head = _MyHead;
        }
    }
};

template <class _Ty>
using threadsafe_stack = atomic_stack <_Ty >;
//template <class _Ty>
//using threadsafe_stack = mutex_stack <_Ty >;

#endif // END OF FOUNDATIONKIT_FOUNDATIONKIT_THREAD_SAFE_STACK._HPP
