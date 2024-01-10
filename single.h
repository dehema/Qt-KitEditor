/**
* @file      template single design
* @datetime  2022-06-03
* @version   v0.0.1
*/
#include <iostream>
using namespace std;

/**
* @class     template_single
* @datetime  2022-06-03
* @version   v0.0.1
*/
#include <iostream>
using namespace std;

template <typename T>
class Single {
public:
    //外部获取单例的接口
    static T& ins()
    {
        static T instance;   //函数静态变量可以实现延时构造。
        return instance;
    }

    //禁止拷贝
    Single(const Single&) = delete;
    Single& operator=(const Single&) = delete;

protected:
    //只有子类才能获取令牌。
    struct Token {};

    //构造和析构函数私有化
    Single() = default;
    virtual ~Single() = default;
};
