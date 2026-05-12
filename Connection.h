#pragma once 
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include "Buffer.h"
#include <memory>
#include <atomic>


//利用只能指针解决conn析构问题
class Connection;
using spConnection=std::shared_ptr<Connection>;
//this报错是因为this穿的是普通指针
//监听的功能
class Connection:public std::enable_shared_from_this<Connection>
{
    private:
        EventLoop *loop_;   //Acceptor对应的事件循环，在构造函数中传入，一个对应一个
        Socket *clientsock_;  //服务端用于监听的socket,在构建函数中创建
        Channel *clientchannel_;    //  Acceptor对应的Channel，在构造函数中创建。
        std::function<void (spConnection)> closecallback_;
        std::function<void (spConnection)> errorcallback_;
        std::function<void(spConnection,std::string &)> slovemessagecallback_;
        //取消完写事件，就可以是发送完毕
        std::function<void (spConnection)> sendCompletecb_;
        
        std::atomic<bool> disconnect_;
        Buffer inputbuffer_;
        Buffer outputbuffer_;
        
    public:
        Connection(EventLoop *loop,Socket *clientsock); //  
        ~Connection();

        int fd() const;
        std::string ip(); //返回ip_
        uint16_t port();    //返回port_

        void closecallback();   //我感觉取名有点问题
        void errorcallback();

        void setcloseback(std::function<void (spConnection)>fn);
        void seterrorback(std::function<void (spConnection)>fn);
        void setslovecb(std::function<void(spConnection,std::string &)>fn);
        void setsendCompletecb(std::function<void (spConnection)>fn);
        void onMessage();

        //可以是把数据先放到outbuffer,然后注册写事件，
        void sendto_ob(const char* data,size_t size);

        void writecallback();
};