// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#ifndef MUDUO_BASE_WEAKCALLBACK_H
#define MUDUO_BASE_WEAKCALLBACK_H

#include <functional>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace muduo
{

// A barely usable WeakCallback
// liujia: __GXX_EXPERIMENTAL_CXX0X__ is to let GCC chekc if -std=c++0x is in effect.
#ifdef __GXX_EXPERIMENTAL_CXX0X__

// FIXME: support std::shared_ptr as well, maybe using template template parameters

template<typename CLASS, typename... ARGS>
class WeakCallback
{
 public:

  WeakCallback(const boost::weak_ptr<CLASS>& object,
               const std::function<void (CLASS*, ARGS...)>& function)
    : object_(object), function_(function)
  {
  }

  // Default dtor, copy ctor and assignment are okay

  void operator()(ARGS&&... args) const
  {
    boost::shared_ptr<CLASS> ptr(object_.lock());
    if (ptr)
    {
      function_(ptr.get(), std::forward<ARGS>(args)...);
    }
    // else
    // {
    //   LOG_TRACE << "expired";
    // }
  }

 private:

  boost::weak_ptr<CLASS> object_;
  std::function<void (CLASS*, ARGS...)> function_;
};

//liujia: 接受两个参数，一个是类CLASS的shared_ptr对象，一个是类的成员函数指针
//这个成员函数指针用来初始化WeakCallback的第二个参数function对象
//构造一个函数对象，即function对象可以用下面两种方法
//法1：function<bool(int)> f; 即这个function对象包装的“函数”或者“函数对象functor”的参数是int，返回值是bool
//法2：template<typename F> function(F f);这个function对象的参数和返回值，与模板参数F的完全一致(也允许默认类型转换)，
//这里的F可以是一个函数或者一个函数对象
//参考： http://www.cnblogs.com/hujian/archive/2012/12/07/2807605.html
template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...))
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                              void (CLASS::*function)(ARGS...) const)
{
  return WeakCallback<CLASS, ARGS...>(object, function);
}

#else  // __GXX_EXPERIMENTAL_CXX0X__

// the C++98/03 version doesn't support arguments.

template<typename CLASS>
class WeakCallback
{
 public:

  WeakCallback(const boost::weak_ptr<CLASS>& object,
               const boost::function<void (CLASS*)>& function)
    : object_(object), function_(function)
  {
  }

  // Default dtor, copy ctor and assignment are okay

  void operator()() const
  {
    boost::shared_ptr<CLASS> ptr(object_.lock());
    if (ptr)
    {
      function_(ptr.get());
    }
    // else
    // {
    //   LOG_TRACE << "expired";
    // }
  }

 private:

  boost::weak_ptr<CLASS> object_;
  boost::function<void (CLASS*)> function_;
};

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)())
{
  return WeakCallback<CLASS>(object, function);
}

template<typename CLASS>
WeakCallback<CLASS> makeWeakCallback(const boost::shared_ptr<CLASS>& object,
                                     void (CLASS::*function)() const)
{
  return WeakCallback<CLASS>(object, function);
}

#endif  // __GXX_EXPERIMENTAL_CXX0X__
}

#endif
