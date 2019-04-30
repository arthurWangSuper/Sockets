#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

#define MAXLINE 4096
static void *
threadFunc(void *arg)
{

	char buff[400];
	int connfd = *((int*)arg);
	int n = recv(connfd, buff, MAXLINE, 0);
	if(n == -1)
	{
			printf("====rst!!!====\r\n");
			close(connfd);
			exit(5);
	}
	else if(n>0)
	{
		buff[n] = '\0';
		printf("recv msg from client: %s\n", buff);
	}
	else
	{
		printf("recv error \r\n");
			//break;
	}
	return NULL;
}

int main(int argc, char** argv)
{
	pthread_t t1;

	int listenfd, connfd;
	struct sockaddr_in servaddr;
	
	char buff[4096];
	int n;
	int s;
	
	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
	printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(6666);

	if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}

	if( listen(listenfd, 10) == -1){
		printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}

	if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
		printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
	}
#if 0
	while(1){
		n = recv(connfd, buff, MAXLINE, 0);
		if(n == -1)
		{
			printf("====rst!!!====\r\n");
			close(connfd);
			exit(5);
		}
		else if(n>0)
		{
			buff[n] = '\0';
			printf("recv msg from client: %s\n", buff);
		}
		else
		{
			printf("recv error \r\n");
			break;
		}
		//send(connfd,buff,n,0);
		//close(connfd);
	}
#endif
	s = pthread_create(&t1, NULL, threadFunc, &connfd);
    if (s != 0)
        printf("pthread_create");

    s = pthread_join(t1, NULL);
    if (s != 0)
        printf("pthread_join");


	close(connfd);
	close(listenfd);
	return 0;
}
