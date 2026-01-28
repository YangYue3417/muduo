#include "muduo/base/Singleton.h"
#include "muduo/base/CurrentThread.h"
#include "muduo/base/Thread.h"

#include <iostream>

class Test : muduo::noncopyable
{
 public:
  Test()
  {
    std::cout << "tid=" << muduo::CurrentThread::tid()
              << ", constructing " << this << std::endl;
  }

  ~Test()
  {
    std::cout << "tid=" << muduo::CurrentThread::tid()
              << ", destructing " << this << ' ' << name_ << std::endl;
  }

  const muduo::string& name() const { return name_; }
  void setName(const muduo::string& n) { name_ = n; }

 private:
  muduo::string name_;
};

class TestNoDestroy : muduo::noncopyable
{
 public:
  // Tag member for Singleton<T>
  void no_destroy();

  TestNoDestroy()
  {
    std::cout << "tid=" << muduo::CurrentThread::tid()
              << ", constructing TestNoDestroy " << this << std::endl;
  }

  ~TestNoDestroy()
  {
    std::cout << "tid=" << muduo::CurrentThread::tid()
              << ", destructing TestNoDestroy " << this << std::endl;
  }
};

void threadFunc()
{
  std::cout << "tid=" << muduo::CurrentThread::tid()
            << ", " << &muduo::Singleton<Test>::instance()
            << " name=" << muduo::Singleton<Test>::instance().name()
            << std::endl;
  muduo::Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
  muduo::Singleton<Test>::instance().setName("only one");
  muduo::Thread t1(threadFunc);
  t1.start();
  t1.join();
  std::cout << "tid=" << muduo::CurrentThread::tid()
            << ", " << &muduo::Singleton<Test>::instance()
            << " name=" << muduo::Singleton<Test>::instance().name()
            << std::endl;
  muduo::Singleton<TestNoDestroy>::instance();
  std::cout << "with valgrind, you should see " << sizeof(TestNoDestroy)
            << "-byte memory leak." << std::endl;
}
