#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

EventLoop::EventLoop():ep_(new Epoll)
 {

 }
 EventLoop::~EventLoop()
{
    delete ep_;
}

void  EventLoop::run()
{
    while(true)
    {
        std::vector<Channel *> channels;   
        //看看这里，可以继续封装，这里都是channel
        channels=ep_->loop();  //这个函数的类型为std::vector<epoll_event>锁一前面要定义一个evs来接受
        for(auto &ch:channels)   //熟悉这种遍历。
        {
            ch->handleevent();
        }
 
    }
}

Epoll* EventLoop::ep()
{
    return ep_;
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

