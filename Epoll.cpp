#include "Epoll.h"
#include <cstdio>
#include <sys/epoll.h>

using namespace std;
 //在构造函数里面创建了epollfd_
Epoll::Epoll()
{
    epollfd_=epoll_create(1);
    if(epollfd_==-1)
    {
        printf("epoll_create fail\n");
        exit(-1);
    }
}
//关闭epollfd_
Epoll::~Epoll()
{
    close(epollfd_);
}


//思考epoll的三个函数功能，进行封装
//把fd和它需要监视的事件添加到红黑树上面。
void Epoll::addfd(int fd,uint32_t op)
{
    struct epoll_event ev;//这里是epoll的数据结构
    ev.data.fd=fd;  //指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回？？？？
    ev.events=op;

    int addnum=epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev);
    if(addnum==-1)
    {
        printf("epoll_ctl() failed(%d).\n",errno);
        exit(-1);
    }
}

//这里很妙使用了stl里面的vector去存放epoll_wait的evs;
//还有一个参数timeout
//运行epoll_wait(),等待事件的发生，已发生的事件用vector返回
std::vector<epoll_event>Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;   //这个的作用是相当于对于private的属性的接口吗？？？？？
    bzero(events_,sizeof(events_));
    int fdnums=epoll_wait(epollfd_,events_,Maxevents,timeout);//这个函数仔细解释,等待监视事件fd又是件发生
        if(fdnums==-1)
        {
            perror("epoll_wait fail\n");
            //循环---》break
            exit(-1) ;
        }
        else if(fdnums==0)
        {
            cout<<"超时"<<endl;
            return evs;
        }
        //这里是没问题的，默认大于0的文件描述符
        for(int i=0;i<fdnums;i++)
        {
            evs.push_back(events_[i]);
        }
        return evs; //重要噶
}