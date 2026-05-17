#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <sys/syscall.h>
#include <unistd.h>

EventLoop::EventLoop():ep_(new Epoll)
 {

 }
 EventLoop::~EventLoop()
{
    //delete ep_;
}

void  EventLoop::run()
{
    
    while(true)
    {
        std::vector<Channel *> channels;
          
        //看看这里，可以继续封装，这里都是channel
        channels=ep_->loop(10*1000);  //这个函数的类型为std::vector<epoll_event>锁一前面要定义一个evs来接受
        if(channels.size()==0)
        {
            epolltimeoutcb_(this);
        }
        else
        {
            for(auto &ch:channels)   //熟悉这种遍历。
            {
                ch->handleevent();
        }
        }
    }
}




void EventLoop::setepolltimeoutcb(std::function<void (EventLoop *)> fn)
{
    epolltimeoutcb_=fn;
}

void EventLoop::updatechannel(Channel *ch)
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)
{
    ep_->removechannel(ch);
}


