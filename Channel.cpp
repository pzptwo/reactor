#include "Channel.h"

using namespace std;
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

Channel::Channel(Epoll *ep,int fd,bool islisten):ep_(ep),fd_(fd),islisten_(islisten)
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
    events_=events_|EPOLLIN;
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
uint32_t Channel::revents()
{
    return revents_;
}
//事件处理函数，epoll_wait()返回的时候，执行它
void Channel::handleevent(Socket *servsock)
{
    if(revents_&EPOLLRDHUP)
    {
        //这里表示对方已关闭
        cout<<"client(eventfd)"<<fd_<<"disconnect"<<endl;
        close(fd_);
    }
    else if(revents_&(EPOLLIN|EPOLLPRI))
    {
        //服务器这边要分为两种
        if(islisten_==true)//这里就要开始accept了,这里就会有新的fd了,//v6改的时候，看看我的这里并没有分两种fd
        {
            
            //这里用指针的原因是在堆区？还是栈忘了哈哈哈哈，就是防止这个{}结束释放clientsock
            //这里要分清楚在accept这还是属于listenfd(我的理解)
            InetAddress clientaddr;//如果用别的构造函数，会咋样
            Socket *clientsock=new Socket(servsock->accept(clientaddr));//(相当于传进来clientfd)
            //打印一下日志
            cout << "accept client: fd=" << clientsock->fd()
                    << ", ip=" << clientaddr.ip()
                    << ", port=" << clientaddr.port()<< endl;
            //为新用户端连接准备读事件，并添加到epoll
            
            
            Channel *clientchannel=new Channel(ep_,clientsock->fd(),false);
            clientchannel->useet();
            clientchannel->enablereading();
            //clientchannel.updatechannel(clientchannel);
        }
        else 
        {
            char buffer[1024];
            while(true)
            {
                bzero(buffer,sizeof(buffer));//这个函数与memset的区别
                ssize_t nread=read(fd_,buffer,sizeof(buffer));//这个函数的赋值？？？

                if(nread>0)
                {
                    cout << "recv from client(eventfd=" <<fd_ << "): " << buffer << endl;
                    //这里服务器在接收到数据，后的处理方式
                    send(fd_,buffer,strlen(buffer),0);
                }
                //错误有好几种，有些需要排除
                else if(nread==-1&&errno==EINTR)     // 读取数据的时候被信号中断，继续读取。
                {
                    continue;
                }
                else if(nread==-1&&((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                {
                    break;
                }
                else if(nread==0)   // 客户端连接已断开，和上面的重复了
                {
                    printf("client(eventfd=%d) disconnected.\n",fd_);
                    close(fd_);            // 关闭客户端的fd。
                    break;
                }
            }
        }
    }
    //后面的events类型
    else if(revents_&EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
    {

    }
    else 
    {
            printf("client(eventfd=%d) error.\n",fd_);
        close(fd_);            // 关闭客户端的fd。
    }
}