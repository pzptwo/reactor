#pragma once 
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

//监听的功能
class Connection 
{
    private:
        EventLoop *loop_;   //Acceptor对应的事件循环，在构造函数中传入，一个对应一个
        Socket *clientsock_;  //服务端用于监听的socket,在构建函数中创建
        Channel *clientchannel_;    //  Acceptor对应的Channel，在构造函数中创建。
    public:
        Connection(EventLoop *loop,Socket *clientsock); //  
        ~Connection();
};