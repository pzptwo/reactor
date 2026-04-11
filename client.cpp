#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>//struct sockaddr_in servaddr;
#include <iostream>

using namespace std;

int main (int argc,char * argv[])
{
    if(argc!=3)
    {
        printf("usage :ip port\n ");//argv顺序存
        printf("./client 192.168.229.128 5000\n");
        return -1;
    }
    int sockfd;

    if((sockfd=(socket(AF_INET,SOCK_STREAM,0)))==-1)
    {   
        printf("socket fail\n");
        // close (sockfd);
        exit(1);//??
    }
    //bind 
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_family=AF_INET;

    if((connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))==-1)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
        
    }
    
    cout<<"connect tcpepoll"<<endl;
    char buf[1024];
    

    for(int i=0;i<1000;i++)
    {
        memset(buf,0,sizeof(buf));
        printf("输入：");
        cin>>buf;
        if((send(sockfd,buf,strlen(buf),0))<=0)
        {
            printf("write fail\n");
            close (sockfd);
            return -1;
        }
        //发送与接收

        memset(buf,0,sizeof(buf));
        if((recv(sockfd,buf,sizeof(buf),0))<=0)
        {
            printf("read fail\n");
            close (sockfd);
            return -1;
        }
        cout<<"recv"<<buf<<endl;
    }
    return 0;
}