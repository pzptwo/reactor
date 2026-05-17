#pragma once 
#include "Epoll.h"
#include <functional>
#include <cstddef>
#include <memory>

class Epoll;

class EventLoop
{
    private:
        std::unique_ptr<Epoll> ep_;
        std::function<void (EventLoop *)> epolltimeoutcb_;

    public:
        EventLoop();
        ~EventLoop();

        void run();
        Epoll *ep();

        void setepolltimeoutcb(std::function<void (EventLoop *)> fn);
        void updatechannel(Channel *ch);

        void removechannel(Channel *ch);

};