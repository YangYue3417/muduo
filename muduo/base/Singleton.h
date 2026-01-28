// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_SINGLETON_H
#define MUDUO_BASE_SINGLETON_H

#include "muduo/base/noncopyable.h"

#include <assert.h>
#include <pthread.h>
#include <type_traits>
#include <stdlib.h> // atexit

namespace muduo
{

namespace detail
{
template<typename T, typename = void>
struct has_no_destroy : std::false_type {};

template<typename T>
struct has_no_destroy<T, std::void_t<decltype(&T::no_destroy)>> : std::true_type {};

template<typename T, typename = void>
struct is_complete : std::false_type {};

template<typename T>
struct is_complete<T, std::void_t<decltype(sizeof(T))>> : std::true_type {};
}  // namespace detail

template<typename T>
class Singleton : noncopyable
{
 public:
  Singleton() = delete;
  ~Singleton() = delete;

  static T& instance()
  {
    pthread_once(&ponce_, &Singleton::init);
    assert(value_ != NULL);
    return *value_;
  }

 private:
  static void init()
  {
    value_ = new T();
    if constexpr (!detail::has_no_destroy<T>::value)
    {
      ::atexit(destroy);
    }
  }

  static void destroy()
  {
    static_assert(detail::is_complete<T>::value,
                  "Singleton::destroy() T must be complete type");
    delete value_;
    value_ = nullptr;
  }

 private:
  static pthread_once_t ponce_;
  static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = nullptr;

}  // namespace muduo

#endif  // MUDUO_BASE_SINGLETON_H
