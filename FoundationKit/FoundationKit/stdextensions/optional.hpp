/****************************************************************************
  Copyright (c) 2015 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#ifndef STD_OPTIONAL_H_
#define STD_OPTIONAL_H_

#include <type_traits>
#include <exception>
#include <string>
#include "stddefines.hpp"

_STD_BEGIN

class nullopt_t { };

class in_place_t { };

class bad_optional_access: public std::logic_error {
 public:
	explicit bad_optional_access(const std::string &what_arg)
		: logic_error(what_arg) { }

	explicit bad_optional_access(const char *what_arg)
		: logic_error(what_arg) { }
};

template<class T>
class optional {
 public:
	typedef T   value_type;

	optional() _NOEXCEPT
		: empty_{true} { }

		optional(nullopt_t) _NOEXCEPT
		: empty_{true} { }

	optional(const optional &other)
		: empty_{other.empty_}
	{
		if (!empty_) {
			new(storage_) T{other.value()};
		}
	}

	optional(optional &&other) _NOEXCEPT
		: empty_{other.empty_}
	{
		if (!empty_) {
			new(storage_) T{std::move(other.value())};
		}
	}
 
	optional(const T &val)
		: empty_{false}
	{
		new(storage_) T{val};
	}

	optional(T &&val) _NOEXCEPT
		: empty_{false}
	{
		new(storage_) T{std::move(val)};
	}

	template<class... Args>
	explicit optional(in_place_t, Args&&... args)
		: empty_{false}
	{
		new(storage_) T{std::forward<Args>(args)...};
	}

	~optional()
	{
		if (!empty_ && sizeof(T)) {
			reinterpret_cast<T*>(storage_)->~T();
		}
	}

	optional & operator=(nullopt_t)_NOEXCEPT
	{
		if (!empty_) {
			reinterpret_cast<T*>(storage_)->~T();
		}
		empty_ = true;

		return *this;
	}

	optional & operator=(const optional &other)
	{
		optional tmp{other};
		swap(tmp);

		return *this;
	}

	optional & operator=(optional &&other) _NOEXCEPT
	{
		if (empty_) {
			if (other.empty_) {
				return *this;
			} else {
				new(storage_) T{std::move(other.value())};
				empty_ = false;
			}
		} else {
			if (other.empty_) {
				reinterpret_cast<T*>(storage_)->~T();
				empty_ = true;
			} else {
				*reinterpret_cast<T*>(storage_) = std::move(other.value());
			}
		}
		return *this;
	}

	const T * operator->() const _NOEXCEPT{
		return reinterpret_cast<const T*>(storage_);
	}

	T * operator->() _NOEXCEPT{
		return reinterpret_cast<T*>(storage_);
	}

	const T & operator*() const _NOEXCEPT {
		return *reinterpret_cast<const T*>(storage_);
	}

	T & operator*() _NOEXCEPT{
		return *reinterpret_cast<T*>(storage_);
	}

	explicit operator bool() const _NOEXCEPT{
		return !empty_;
	}

	T & value() { 
		if (empty_) {
			throw bad_optional_access{"call value on empty object"};
		}
		return **this;
	}

	const T & value() const {
		if (empty_) {
			throw bad_optional_access{"call value on empty object"};
		}
		return **this;
	}

	template<class U>
	T value_or(U &&value) const {
		return empty_ ? value : **this;
	}

	void swap(optional &other) _NOEXCEPT
	{
		if (empty_ && other.empty_) {
			return;
		}

		if (!empty_ && !other.empty_) {
			swap(**this, *other);
			return;
		}

		optional * in = empty_ ? this : &other;
		optional * un = empty_ ? &other : this;

		new(un->storage_) T{std::move(**in)};
		reinterpret_cast<T*>(in->storage_)->~T();
		un->empty_ = false;
		in->empty_ = true;
	}

	template<class... Args>
	void emplace(Args&&... args)
	{
		if (!empty_) {
			reinterpret_cast<T*>(storage_)->~T();
			empty_ = true;
		}
		new(storage_) T{std::forward<Args>(args)...};
		empty_ = false;
	}

 private:
	 using aligned_type = typename _STD aligned_storage<sizeof(T), _STD alignment_of<T>::value>::type;
	 aligned_type storage_[1];
	bool empty_;
};

template<class T>
bool operator==(const optional<T> &lhs, const optional<T> &rhs) {
	if (bool(lhs) != bool(rhs)) {
		return false;
	}
	if (!bool(lhs) && !bool(rhs)) {
		return true;
	}
	return *lhs == *rhs;
}

template<class T>
bool operator<(const optional<T> &lhs, const optional<T> &rhs) {
	if (!bool(rhs)) {
		return false;
	}
	if (!bool(lhs)) {
		return true;
	}
	return std::less<T>{}(*rhs, *lhs);
}

template<class T>
bool operator==(const optional<T> &opt, nullopt_t) _NOEXCEPT{
	return !bool(opt);
}

template<class T>
bool operator==(nullopt_t, const optional<T> &opt) _NOEXCEPT{
	return !bool(opt);
}

template<class T>
bool operator<(const optional<T> &opt, nullopt_t) _NOEXCEPT{
	return !bool(opt);
}

template<class T>
bool operator<(nullopt_t, const optional<T> &opt) _NOEXCEPT{
	return !bool(opt);
}

template<class T>
bool operator==(const optional<T> &opt, const T &value) {
	return bool(opt) ? *opt == value : false;
}

template<class T>
bool operator==(const T &value, const optional<T> &opt) {
	return bool(opt) ? *opt == value : false;
}

template<class T>
bool operator<(const optional<T> &opt, const T &value) {
	return bool(opt) ? std::less<T>{}(*opt, value) : true;
}

template<class T>
bool operator<(const T &value, const optional<T> &opt) {
	return bool(opt) ? std::less<T>{}(*opt, value) : false;
}

_STD_END

#endif // #ifndef STD_OPTIONAL_H_

