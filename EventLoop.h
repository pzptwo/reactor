#pragma once 
#include "Epoll.h"
#include <functional>
#include <cstddef>

class Epoll;

class EventLoop
{
    private:
        Epoll *ep_=nullptr;
        std::function<void (EventLoop *)> epolltimeoutcb_;

    public:
        EventLoop();
        ~EventLoop();

        void run();
        Epoll *ep();

        void setepolltimeoutcb(std::function<void (EventLoop *)> fn);
        void updatechannel(Channel *ch);
};