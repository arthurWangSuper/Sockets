#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include <sys/epoll.h>
#include <fcntl.h>


#define MAXLINE 4096
#define MAX_EVENTS 5

int main(int argc, char** argv)
{
    int    listenfd, connfd;
    struct sockaddr_in     servaddr;
    char    buff[4096];
    int     n,i;
	int flag =1;
	int len = sizeof(int);
	int epfd;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];
	
	epfd = epoll_create(MAX_EVENTS);
	if(epfd == -1)
		printf("epoll_create failed!!!");

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
    	printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6666);

	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1) 
	{ 
	    printf("setsockopt");
	    exit(1); 
    } 			
	//signal 一次性
	//ignore SIGPIPE
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;

	sigaction(SIGPIPE,&sa,0);


    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
    	printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }

    if( listen(listenfd, 10) == -1){
    	printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
    	exit(0);
    }
	
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if(epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev) == -1)
	{
		printf("epoll ctl");
	}

	int ready = epoll_wait(epfd,evlist,MAX_EVENTS,-1);
	if(ready == -1){
		if(errno == EINTR){
			printf("interupt epoll");
		}else{
			printf("epoll wait");
		}
	}
	printf("Ready: %d",ready);

	for(i = 0;i<ready;i++)
	{
		if(evlist[i].events&EPOLLIN)
		{
			printf("client is coming");
		}
	}

    printf("======waiting for client's request======\n");
    if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
    }
while(1){
    n = recv(connfd, buff, MAXLINE, 0);
    if(n == -1)
    {
	printf("====rst===!!!\r\n");
    	close(connfd);
	close(listenfd);
	exit(100);
    }
    buff[n] = '\0';
    printf("recv msg from client: %s\n", buff);
}
	char *sendbuf = "byebye";
	//test SIGPIPE 
	//first send rcv RST
	//second send rcv SIGPIPE
	send(connfd,sendbuf,strlen(sendbuf),0);
	
	send(connfd,sendbuf,strlen(sendbuf),0);
	sleep(10);
	close(connfd);

        close(listenfd);
	return 0;
}
