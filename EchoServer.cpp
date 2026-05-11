#include "EchoServer.h"
#include "Connection.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include <functional>
#include <iostream>
#include <ratio>

using namespace std;

EchoServer::EchoServer(const std::string ip,const uint16_t port,int subthreadNum,int workthreadNum):tcpserver_(ip,port,subthreadNum),
                        threadpool_(workthreadNum,"WORKS")
{
    tcpserver_.setnewConnectioncb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver_.setclosecb(std::bind(&EchoServer::HandleClose,this,std::placeholders::_1));
    tcpserver_.seterrorcb(std::bind(&EchoServer::HandleError,this,std::placeholders::_1));
    tcpserver_.setslovemessagecb(std::bind(&EchoServer::HandleSlovemessage,this,std::placeholders::_1,std::placeholders::_2));
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
void EchoServer::HandleNewConnection(Connection *conn)
{   
   //printf("HandleNewConnection thread is %ld\n",syscall(SYS_gettid));
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
void EchoServer::HandleSlovemessage(Connection* conn,std::string & message)
{
    //这里就是工作线程的开始。
    //printf("HandleSlovemessage thread is %ld\n",syscall(SYS_gettid));
    //把任务函数放入线程池里面的任务队列,由于我一开始的打包是bind.....<void ()>,无参，该函数有参，打包，有参的话，std::placeholders占位符
    threadpool_.addtask(std::bind(&EchoServer::onworkmessage,this,conn,message));    
}


void EchoServer::onworkmessage(Connection* conn,std::string & message)
{
    message="reply"+message;
    conn->sendto_ob(message.data(),message.size()); 
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