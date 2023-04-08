#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "./SDK/Sqlite3/include/CppSqlite3.h"
#include"./SDK/SMTP/CSmtp.h"
#include "./Global/inc/Base64.h"
#include "./Global/inc/spdlog/spdlog.h"
#include "./Global/inc/spdlog/sinks/basic_file_sink.h"
#include "./SDK/Http/httplib.h"
#include <memory>
#include <future>
#include <thread>
#include <queue>

using namespace std;
using namespace spdlog;

std::string StringToUTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
    ZeroMemory(pwBuf, nwLen * 2 + 2);

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 10];
    ZeroMemory(pBuf, nLen + 10);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen + 9, NULL, NULL);

    std::string retStr(pBuf);

    delete[]pwBuf;
    delete[]pBuf;

    pwBuf = NULL;
    pBuf = NULL;

    return retStr;
}

string Utf8ToGbk(const char* src_str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK) delete[] wszGBK;
    if (szGBK) delete[] szGBK;
    return strTemp;
}

class Test {
public:
    Test() { cout << "Test的构造函数..." << endl; }
    ~Test() { 
        if (ptrName)
            delete[] ptrName;
        cout << "Test的析构函数..." << endl; }

    int getDebug() {
        ptrName = new char[124]{3};

        try
        {
            throw 1;
        }
        catch (int a)
        {
            a = 1;
            return a;
        }
        return this->debug; }

private:
    int debug = 20;
    char* ptrName;
};

class Base {
public:
    int value1;
    int value2;
    Base() {
        value1 = 1;
    }
    Base(int value) : Base() { // 委托 Base() 构造函数
        value2 = value;
    }

    virtual int ShowFunc() { return -1; };

    virtual double DecFunc() final { return 0.4; };
};
class Subclass : public Base {
public:
    using Base::Base; // 继承构造

    int ShowFunc() override { return 0; };
    // int Add()override { return 0; };

   // double DecFunc();
};

template <typename T, typename R>
auto add(T nCount, R dbValue)
{
    std::string strFunName("This is a Demo!");
    double allValue = nCount + dbValue;
    auto t = make_tuple(allValue, strFunName);
    return t;
}

template<typename T, typename... Ts>
auto printf3(T value, Ts... args) {
    std::cout << value << std::endl;
    (void)std::initializer_list<T>{([&args] {
        std::cout << args << std::endl;
        }(), value)...};
}

void lambda_value_capture() {
    int value = 1;
    auto copy_value = [value] {
        return value;
    };
    value = 100;
    auto stored_value = copy_value();
    std::cout << "stored_value = " << stored_value << std::endl;
    // 这时, stored_value == 1, 而 value == 100.
    // 因为 copy_value 在创建时就保存了一份 value 的拷贝
}

int foo(int a, int b, int c) {
    
    return a + b + c;
}

void reference(std::string& str) {
    std::cout << "左值" << std::endl;
}
void reference(std::string&& str) {
    std::cout << "右值" << std::endl;
}

class A {
public:
    int* pointer;
    A() :pointer(new int(1)) {
        std::cout << "构造" << pointer << std::endl;
    }
    A(A& a) :pointer(new int(*a.pointer)) {
        std::cout << "拷贝" << pointer << std::endl;
    } // 无意义的对象拷贝
    A(A&& a) noexcept :pointer(a.pointer) {
        a.pointer = nullptr;
        std::cout << "移动" << pointer << std::endl;
    }
    ~A() {
        std::cout << "析构" << pointer << std::endl;
        delete pointer;
    }
};
// 防止编译器优化
A return_rvalue(bool test) {
    A a, b;
    if (test) return a; // 等价于 static_cast<A&&>(a);
    else return b;     // 等价于 static_cast<A&&>(b);
}

class Flower
{
public:
    virtual void display()//基类的虚函数，形成动态绑定，永远不会运行“动物”，Flower类也从1个字节变成了4个字节（指针）
        //只有在程序运行的时候才知道speak函数具体运行哪个子类的speak函数
    {
        cout << "动物！" << endl;
    }
};

class Rose :public Flower
{
public:
    void display()//派生类重写了基类的虚函数，覆盖了原来虚函数表中的地址
    {
        cout << "玫瑰" << endl;
    }
};

void fun(Flower& f1)//派生类的对象作为实参，形参可以用基类的引用或者指针来接收，目的在于实现多态性
{//实现了派生类对象向基类类型的转换
    f1.display();//加上virtual之后，这一行就要去看你f1派生对象的类型，去调用相对应的display函数
}

// template<typename T>
// class smart_ptr {
// public:
//     explicit smart_ptr(
//         T* ptr = nullptr)
//         : ptr_(ptr) {}
// 
//     smart_ptr(smart_ptr&& other) { ptr_ = other.release(); }
// 
//     ~smart_ptr()
//     {
//         delete ptr_;
//     }
// 
//     //smart_ptr& operator=(smart_ptr& rhs) { smart_ptr(rhs).swap(*this); return *this; }
//     smart_ptr& operator=(smart_ptr rhs) { rhs.swap(*this); return *this; }
// 
//     T& operator*() const { return *ptr_; } 
//     T* operator->() const { return ptr_; }
//     operator bool() const { return ptr_; }
// 
//     T* get() const { return ptr_; }
//     T* release() { T* ptr = ptr_; ptr_ = nullptr; return ptr; }
// 
//     void swap(smart_ptr& rhs) { using std::swap; swap(ptr_, rhs.ptr_); }
// 
// private:
//     T* ptr_;
// };



class shared_count {
public:
    shared_count() : count_(1) {}
    void add_count()
    {
        ++count_;
    }
    long reduce_count()
    {
        return --count_;
    }
    long get_count() const
    {
        return count_;
    }

private:
    long count_;
};

template <typename T>
class smart_ptr {
public:
    template <typename U>
    friend class smart_ptr;

    explicit smart_ptr(T* ptr = nullptr)
        : ptr_(ptr)
    {
        if (ptr) {
            shared_count_ =
                new shared_count();
        }
    }
    ~smart_ptr()
    {
        if (ptr_ &&
            !shared_count_
            ->reduce_count()) {
            delete ptr_;
            delete shared_count_;
        }
    }

    smart_ptr(const smart_ptr& other)
    {
        ptr_ = other.ptr_;
        if (ptr_) {
            other.shared_count_
                ->add_count();
            shared_count_ =
                other.shared_count_;
        }
    }

    template <typename U>
    smart_ptr(const smart_ptr<U>& other)
    {
        ptr_ = other.ptr_;
        if (ptr_) {
            other.shared_count_
                ->add_count();
            shared_count_ =
                other.shared_count_;
        }
    }
    template <typename U>
    smart_ptr(smart_ptr<U>&& other)
    {
        ptr_ = other.ptr_;
        if (ptr_) {
            shared_count_ =
                other.shared_count_;
            other.ptr_ = nullptr;
        }
    }


    long use_count() const
    {
        if (ptr_) {
            return shared_count_
                ->get_count();
        }
        else {
            return 0;
        }
    }

    T& operator*() const noexcept
    {
        return *ptr_;
    }
    T* operator->() const noexcept
    {
        return ptr_;
    }
    operator bool() const noexcept
    {
        return ptr_;
    }


private:
    T* ptr_;
    shared_count* shared_count_;
};

template <typename T>
void swap(smart_ptr<T>& lhs,
    smart_ptr<T>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename T, typename U>
smart_ptr<T> static_pointer_cast(
    const smart_ptr<U>& other) noexcept
{
    T* ptr = static_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> reinterpret_pointer_cast(
    const smart_ptr<U>& other) noexcept
{
    T* ptr = reinterpret_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> const_pointer_cast(
    const smart_ptr<U>& other) noexcept
{
    T* ptr = const_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

template <typename T, typename U>
smart_ptr<T> dynamic_pointer_cast(
    const smart_ptr<U>& other) noexcept
{
    T* ptr = dynamic_cast<T*>(other.get());
    return smart_ptr<T>(other, ptr);
}

class shape {
public:
    virtual ~shape() {}
};

class circle : public shape {
public:
    ~circle() { puts("~circle()"); }
};


template <int n>
struct factorial {
    static const int value =
        n * factorial<n - 1>::value;
};

template <>
struct factorial<0> {
    static const int value = 1;
};

int main(int argc, char* argv[])
{
    //     int nCount = 897;
    //     char cValue = 5, cShow = 56;
    //     lambda_value_capture();
    //     cValue--;
    //     auto ptrFunc = [nCount](char* pValue) ->int { return nCount + *pValue; };
    //     double nTemp = ptrFunc(&cShow);
    // 
    // 
    //     auto bindFoo = std::bind(foo, std::placeholders::_1, std::placeholders::_2, 2);
    //     // 这时调用 bindFoo 时，只需要提供第一个参数即可
    //     bindFoo(1,5);
    // 
    //     std::string lv1 = "string,"; // lv1 是一个左值
    // // std::string&& r1 = lv1; // 非法, 右值引用不能引用左值
    //     std::string&& rv1 = std::move(lv1); // 合法, std::move可以将左值转移为右值
    //     std::cout << rv1 << std::endl; // string,
    // 
    //     const std::string& lv2 = lv1 + lv1; // 合法, 常量左值引用能够延长临时变量的生命周期
    //     // lv2 += "Test"; // 非法, 常量引用无法被修改
    //     std::cout << lv2 << std::endl; // string,string,
    // 
    //     std::string&& rv2 = lv1 + lv2; // 合法, 右值引用延长临时对象生命周期
    //     rv2 += "Test"; // 合法, 非常量引用能够修改临时变量
    //     std::cout << rv2 << std::endl; // string,string,string,Test
    // 
    //     reference(rv2); // 输出左值
    // 
    // 
    //     A obj = return_rvalue(false);
    //     std::cout << "obj:" << std::endl;
    //     std::cout << obj.pointer << std::endl;
    //     std::cout << *obj.pointer << std::endl;


    //     // 将一个返回值为7的 lambda 表达式封装到 task 中
    //     // std::packaged_task 的模板参数为要封装函数的类型
    //     std::packaged_task<int()> task([]() {return 7; });
    //     // 获得 task 的期物
    //     std::future<int> result = task.get_future(); // 在一个线程中执行 task
    //     std::thread(std::move(task)).detach();
    //     std::cout << "waiting...";
    //     result.wait(); // 在此设置屏障，阻塞到期物的完成
    //     // 输出执行结果
    //     std::cout << "done!" << std::endl << "future result is "
    //         << result.get() << std::endl;


    //     std::queue<int> produced_nums;
    //     std::mutex mtx;
    //     std::condition_variable cv;
    //     bool notified = false;  // 通知信号
    // 
    //     // 生产者
    //     auto producer = [&]() {
    //         for (int i = 0; ; i++) {
    //             std::this_thread::sleep_for(std::chrono::milliseconds(900));
    //             std::unique_lock<std::mutex> lock(mtx);
    //             std::cout << "producing " << i << std::endl;
    //             produced_nums.push(i);
    //             notified = true;
    //             cv.notify_all(); // 此处也可以使用 notify_one
    //         }
    //     };
    //     // 消费者
    //     auto consumer = [&]() {
    //         while (true) {
    //             std::unique_lock<std::mutex> lock(mtx);
    //             while (!notified) {  // 避免虚假唤醒
    //                 cv.wait(lock);
    //             }
    //             // 短暂取消锁，使得生产者有机会在消费者消费空前继续生产
    //             lock.unlock();
    //             // 消费者慢于生产者
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //             lock.lock();
    //             while (!produced_nums.empty()) {
    //                 std::cout << "consuming " << produced_nums.front() << std::endl;
    //                 produced_nums.pop();
    //             }
    //             notified = false;
    //         }
    //     };
    // 
    //     // 分别在不同的线程中运行
    //     std::thread p(producer);
    //     std::thread cs[2];
    //     for (int i = 0; i < 2; ++i) {
    //         cs[i] = std::thread(consumer);
    //     }
    //     p.join();
    //     for (int i = 0; i < 2; ++i) {
    //         cs[i].join();
    //     }

//     //“指针常量”即一个指针变量，该变量不能被赋值，而指针指向的内存单元的内容是可以改变的；
//     //重点在最后的“常量”
//     int* const ptr = &nCount;
//     *ptr = 45;
//     
//     
//     //“常量指针”即一个指向常量的指针，指针变量本身可以赋值，而指针指向的内存单元的内容是不可以被重新赋值的；
//     //重点在最后的“指针”
//     const int* ptrToConst;
//     //*ptrToConst = 34; error


    smart_ptr<circle> ptr1(new circle());
    printf("use count of ptr1 is %ld\n",
        ptr1.use_count());
    smart_ptr<shape> ptr2;
    printf("use count of ptr2 was %ld\n",
        ptr2.use_count());
    ptr2 = ptr1;
    printf("use count of ptr2 is now %ld\n",
        ptr2.use_count());
    if (ptr1) {
        puts("ptr1 is not empty");
    }

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    // Set the default logger to file logger
    auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic.txt");
    spdlog::set_default_logger(file_logger);

    return 0;

}