#pragma once 
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include <functional>

//监听的功能
class Connection 
{
    private:
        EventLoop *loop_;   //Acceptor对应的事件循环，在构造函数中传入，一个对应一个
        Socket *clientsock_;  //服务端用于监听的socket,在构建函数中创建
        Channel *clientchannel_;    //  Acceptor对应的Channel，在构造函数中创建。
        std::function<void (Connection *)> closecallback_;
        std::function<void (Connection *)> errorcallback_;
    public:
        Connection(EventLoop *loop,Socket *clientsock); //  
        ~Connection();

        int fd() const;
        std::string ip(); //返回ip_
        uint16_t port();    //返回port_

        void closecallback();   //我感觉取名有点问题
        void errorcallback();

        void setcloseback(std::function<void (Connection *)>fn);
        void seterrorback(std::function<void (Connection *)>fn);
};