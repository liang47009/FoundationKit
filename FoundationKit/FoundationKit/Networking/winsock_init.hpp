#ifndef LOSEMYMIND_WINSOCK_INIT_HPP
#define LOSEMYMIND_WINSOCK_INIT_HPP
# pragma once
#include "FoundationKit/GenericPlatformMacros.h"

#if (TARGET_PLATFORM == PLATFORM_WINDOWS)

NS_FK_BEGIN

class winsock_init_base
{
protected:
  // Structure to track result of initialisation and number of uses. POD is used
  // to ensure that the values are zero-initialised prior to any code being run.
  struct data
  {
    long init_count_;
    long result_;
  };
  static void startup(data& d, unsigned char major, unsigned char minor);
  static void cleanup(data& d);
  static void throw_on_error(data& d);
};

template <int Major = 2, int Minor = 0>
class winsock_init : private winsock_init_base
{
public:
  winsock_init(bool allow_throw = true)
  {
    startup(data_, Major, Minor);
    if (allow_throw)
      throw_on_error(data_);
  }

  winsock_init(const winsock_init&)
  {
    startup(data_, Major, Minor);
    throw_on_error(data_);
  }

  ~winsock_init()
  {
    cleanup(data_);
  }

private:
  static data data_;
};

template <int Major, int Minor>
winsock_init_base::data winsock_init<Major, Minor>::data_;

// Static variable to ensure that winsock is initialised before main, and
// therefore before any other threads can get started.
static const winsock_init<>& winsock_init_instance = winsock_init<>(false);

NS_FK_END

# include "winsock_init.ipp"

#endif //TARGET_PLATFORM == PLATFORM_WINDOWS

#endif // LOSEMYMIND_WINSOCK_INIT_HPP



