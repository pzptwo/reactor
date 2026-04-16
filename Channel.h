#pragma once
#include <cstdint>
#include <sys/epoll.h>
#include "Epoll.h"

class Epoll;
//这个Channel的作用不再是底层，而是更细致的“状态管理”,
// 与Epoll是协作的关系，一个Channel对应一个Epoll,一个Epoll对应多个Channel

class Channel
{
    private:
        int fd_;
        Epoll *ep_; //说明channel 是属于那个红黑树
        bool inepoll_=false;    //Channel是否已经添加到Epoll树上
        //需要分为event_ revent_ 是由于epoll_ctl（）是传入参数，epoll_wait（）是传出参数
        uint32_t events_;    //fd_需要监听的事件listenfd和clientfd需要监听EPOLLIN ,但是clientfd还需要监听EPOLLOUT
        uint32_t revents_;   //fd_已发生的事件
    public:
        Channel(Epoll *ev,int fd);
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