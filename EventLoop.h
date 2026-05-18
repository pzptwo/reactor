#pragma once 
#include "Channel.h"
#include "Epoll.h"
#include <functional>
#include <cstddef>
#include <memory>
#include <sys/types.h>
#include <thread>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/timerfd.h>      // 定时器需要包含这个头文件。
class Epoll;

class EventLoop
{
    private:
        std::unique_ptr<Epoll> ep_;
        std::function<void (EventLoop *)> epolltimeoutcb_;
        pid_t threadspid_;

        //增加在WORK线程时候绑上的任务队列
        std::queue<std::function<void ()>> taskqueue_;
        std::mutex mutex_;

        //使用eventfd
        int wakefd_;

        //这里并没有把eventfd挂到红黑树上
        std::unique_ptr<Channel> wakechannel_;
        //这里c++提供了一种定时器的类，所以可以喝epoll使用,这里的用法查就可以，（还有类似的信号集机制）
        int timerfd_;

        //将fd_对应的channel挂到epoll树上
        std::unique_ptr<Channel> timerfdchannel_;

        //不同的线程超时的处理方法不同
        bool mainloop_;
    public:
        EventLoop(bool mainloop);
        ~EventLoop();

        void run();
        Epoll *ep();

        void setepolltimeoutcb(std::function<void (EventLoop *)> fn);
        void updatechannel(Channel *ch);

        void removechannel(Channel *ch);
        //这里很妙，因为run就是在IO线程循环里面的
        bool isinloop();

        //设置添加队列的函数
        void setinqueue(std::function<void ()> fn);
        //唤醒事件循环（这里杯阻塞在epoll_wait）
        void wakeIO();

        //唤醒事件循环后，就要执行任务了（消费任务）
        void handlewakeIO ();

        //定时器到了之后，就要执行的函数
        void handletime();
};