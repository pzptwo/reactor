#pragma once 
#include "Epoll.h"
#include <functional>
#include <cstddef>
#include <memory>
#include <sys/types.h>
#include <thread>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>

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
    public:
        EventLoop();
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
};