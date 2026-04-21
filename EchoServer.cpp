#include "EchoServer.h"
#include "Connection.h"
#include "TcpServer.h"
#include <functional>
#include <iostream>
#include <ratio>

using namespace std;

EchoServer::EchoServer(const std::string ip,const uint16_t port):tcpserver_(ip,port)
{
    tcpserver_.setnewConnectioncb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver_.setclosecb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver_.seterrorcb(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
    tcpserver_.setslovemessagecb(std::bind(&EchoServer::HAndleSlovemessage,this,std::placeholders::_1,std::placeholders::_2));
    tcpserver_.setsendCompletecb(std::bind(&EchoServer::HandleSendComplete,this,std::placeholders::_1));
    //tcpserver_.setepolltimeoutcb(std::bind(&EchoServer::HandleEpolltimeout,this,std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}
//调用loop的run,相当于*loop_的接口，裕兴事件循环
void EchoServer::start()
{
    tcpserver_.start();
}  
 //处理新连接上来的
void EchoServer::HangleNewConnection(Connection *conn)
{   
    cout<<"New Connection Come in "<<endl;

}
//我感觉取名有点问题
void EchoServer::HandleClose(Connection *conn)
{
    cout<<"EchoServer conn close"<<endl;
}  

void EchoServer::HandleError(Connection * conn)
{
    cout<<"EchoServer conn error"<<endl;
}
void EchoServer::HAndleSlovemessage(Connection* conn,std::string message)
{
    message="reply"+message;
    //发送模式为头加内容
    int len=message.size();
    //这里用string，char都行
    //相当于进行增加报头，但是是拷贝构造？？？
    std::string tmpbuf((char *)&len,sizeof(len));
    tmpbuf.append(message);
    //send(conn->fd(),tmpbuf.data(),tmpbuf.size(),0);   //对于Socket，这里有隐藏的bug
    conn->sendto_ob(tmpbuf.data(),tmpbuf.size()); 

}
//对于最上层，要知道，数据已经发送完毕
void EchoServer::HandleSendComplete(Connection *conn)
{
    cout<<"Message Send cpmplete"<<endl;
}

//对于最上层，要知道，是否超时,当Channel为空时，而且因为在eventloop里面，所以要说明是哪一个loop
// void EchoServer::HandleEpolltimeout(EventLoop *loop)
// {
//     cout<<"EchoServer timeout"<<endl;
// }