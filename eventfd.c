/*from man eventfd eg*/
/*int eventfd(unsigned int initval,int flags)*/
/*flags: EFD_CLOEXEC,EFD_NONBLOCK,EFD_SEMPHORE*/
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>

#define handle_error(msg)\
	do{perror(msg);exit(EXIT_FAILURE);} while(0)


void producer()
{

}
void consumer()
{

}

int main(int argc,char *argv[])
{
	int efd,j;
	uint64_t u;
	ssize_t s;

	if(argc<2){
		fprintf(stderr,"Usage: %s <num>...\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	efd = eventfd(0,0);
	if(efd == -1)
		handle_error("eventfd");

	switch(fork()){
		case 0:
			for(j = 1;j<argc;j++){
				printf("Child writing %s to efd\n",argv[j]);
				u = strtoull(argv[j],NULL,0);
				//a write call adds the 8-byte integer value supplied in its buffer to the counter
				s = write(efd,&u,sizeof(uint64_t));
				if(s != sizeof(uint64_t))
					handle_error("write");
			}
			printf("Child completed write loop\n");

			exit(EXIT_SUCCESS);
		default:
			sleep(2);

			printf("Parent about to read\n");

			s = read(efd,&u,sizeof(uint64_t));
			if(s != sizeof(uint64_t))
				handle_error("read");
			printf("Parent read %llu (0xllx) from efd\n",(unsigned long long) u,(unsigned long long) u);

			exit(EXIT_SUCCESS);
		case -1:
			handle_error("fork");
	}	
	return 0;
}
