#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
//#include <signal.h>

struct _cfdIdx {
	char *buf;
	int len;
};

struct _denyset {
	char ip[40];
};

struct _ipset {
	char ipStatus; //0=off, 1=0.0.0.0 ,4=ipv4,6=ipv6
	char bind[41];
	char connect[16];
	int bindPort;
	int connectPort;
};
char *regD;
int *SIdx;
pthread_t *worker;
struct _denyset deny[2];
struct _cfdIdx *cfdIdx;
struct _ipset ipset;
int bindFd;
int kepollFd;

int cfdclose(int fd) {
		close(fd);
		free(cfdIdx[fd].buf);
		cfdIdx[fd].buf=NULL;
		epoll_ctl(kepollFd, EPOLL_CTL_DEL, fd, NULL);
		return 0;
}

void _log(char *str1,char *str2,char *str3){
	char timebuf[256];
	time_t time1;
	struct tm *time2;
	time(&time1);
	time2=localtime(&time1);
	memcpy(timebuf,asctime(time2),256);
	strtok(timebuf, "\n");
	printf("reforward: [time]:[%s], %s%s%s",timebuf,str1,str2,str3);
}

int refdlink(){
	struct sockaddr_in linkaddr;
	int cfd;
	
	cfd=socket(AF_INET, SOCK_STREAM, 0);
	
	if (cfd <0){
	_log("set remote socket, ","error"," \n");
	return -1;}
	memset(&linkaddr,0,sizeof(linkaddr));
	linkaddr.sin_family = AF_INET;
	linkaddr.sin_port = htons(ipset.connectPort);
	
		inet_pton(AF_INET,ipset.connect, &(linkaddr.sin_addr));
	
	
	if (connect(cfd,(struct sockaddr *)&linkaddr, sizeof(linkaddr)) < 0) {
		_log("connect remote ","err"," \n");
		return -1;
	}
	return cfd;
}