#pragma once 
#include "Epoll.h"
#include <cstddef>

class Epoll;

class EventLoop
{
    private:
        Epoll *ep_=nullptr;

    public:
        EventLoop();
        ~EventLoop();

        void run();
        Epoll *ep();

        void updatechannel(Channel *ch);
};