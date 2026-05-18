#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <cstdint>
#include <functional>
#include <mutex>
#include <sys/syscall.h>
#include <unistd.h>

EventLoop::EventLoop():ep_(new Epoll),wakefd_(eventfd(0,EFD_NONBLOCK)),wakechannel_ (new Channel(this,wakefd_ ))
{
    wakechannel_->setreadback(std::bind(&EventLoop::handlewakeIO,this));
    wakechannel_->enablereading();
}
 EventLoop::~EventLoop()
{
    //delete ep_;
}

void  EventLoop::run()
{
    threadspid_=syscall(SYS_gettid);
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

bool EventLoop::isinloop()
{
    return threadspid_==syscall(SYS_gettid);
}

//设置添加队列的函数
void EventLoop::setinqueue(std::function<void ()>fn)
{
    //把任务加入到队列里面需要锁
    {
        std::lock_guard<std::mutex>gd(mutex_);
        taskqueue_.push(fn);
    }

    //有了任务就要唤醒IO事件循环
    wakeIO();
}

//唤醒eventfd
void EventLoop::wakeIO()
{
    uint64_t val=1;
    write(wakefd_,&val,sizeof(val));
}

 //唤醒事件循环后，就要执行任务了（消费任务）
 void EventLoop::handlewakeIO ()
 {
    printf("handlewakeIO thread is %ld\n",syscall(SYS_gettid));
    uint64_t n;
    //要把eventfd的读出来，否则在水平触发会一直触发
    read(wakefd_,&n,sizeof(n));

    std::function<void()> tmp;  //接收执行任务的变量
    //这里就要开始（执行任务）
    std::lock_guard<std::mutex> gd(mutex_);
    while(taskqueue_.size()>0)
    {
        tmp=std::move(taskqueue_.front());
        taskqueue_.pop();
        tmp();
    }

 }

