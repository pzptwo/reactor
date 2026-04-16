#include "Channel.h"

/*
class Channel
{
    private:
        int fd_;
        Epoll *ev_; //说明channel 是属于那个红黑树
        bool inepoll_=false;    //Channel是否已经添加到Epoll树上
        //需要分为event_ revent_ 是由于epoll_ctl（）是传入参数，epoll_wait（）是传出参数
        uint32_t event_;    //fd_需要监听的事件listenfd和clientfd需要监听EPOLLIN ,但是clientfd还需要监听EPOLLOUT
        uint32_t revent_;   //fd_已发生的事件
    public:
        Channel(Epoll ev,int fd);
        ~Channel();
        //这个可以从Channel对应(管理)一个fd，fd相关的状态
        int fd();
        void useet();   //采用边缘触发
        void enablereading();//让epoll_wait监听fd_的事件（这里是读）
        void setinepoll();  //把inepoll_成员设置为true;
        void setrevent(uint32_t ev);   //外面的将uint32_t ev传进来，给revent_赋值
        bool inepoll(); //返回inepoll_成员
        uint32_t events();
        uint32_t revent();
};
*/

Channel::Channel(Epoll *ep,int fd):ep_(ep),fd_(fd)
{

}

Channel::~Channel()
{

}
//这个可以从Channel对应(管理)一个fd，fd相关的状态
int Channel::fd()
{
    return fd_;
}
//采用边缘触发
void Channel::useet()
{
    //注意因为是宏，所以用位运算
    events_|=EPOLLET;
}
//让epoll_wait监听fd_的事件（这里是读）
void Channel::enablereading()
{
    events_=events_|=EPOLLIN;
    ep_->updatechannel(this);
}
//把inepoll_成员设置为true;
void Channel::setinepoll()
{
    inepoll_=true;
} 
//外面的将uint32_t ev传进来，给revent_赋值
void Channel::setrevent(uint32_t ev)
{
    revents_=ev;
}  
//返回inepoll_成员
bool Channel::inepoll()
{
    return inepoll_;
}

uint32_t Channel::events()
{
    return events_;
}
uint32_t Channel::revent()
{
    return revents_;
}