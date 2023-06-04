
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
struct _cfdIdx {
	char *buf;
	int len;
	int rlen;
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

int *SIdx;
struct _denyset deny[2];
struct _cfdIdx *cfdIdx;
struct _ipset ipset;
int kepollFd;
int refdlink(){
	struct sockaddr_in linkaddr;
	int cfd;
	
	cfd=socket(AF_INET, SOCK_STREAM, 0);
	
	if (cfd <0){//log("set remote socket, ","error"," \n");
	return -1;}
	memset(&linkaddr,0,sizeof(linkaddr));
	linkaddr.sin_family = AF_INET;
	linkaddr.sin_port = htons(ipset.connectPort);
	
		inet_pton(AF_INET,ipset.connect, &(linkaddr.sin_addr));
	
	
	if (connect(cfd,(struct sockaddr *)&linkaddr, sizeof(linkaddr)) < 0) {
		//log("connect remote err"," "," \n");
		return -1;
	}
	return cfd;
}
void ioctlsocketX(int fd)
{
	int tmp;
	tmp = fcntl(fd, F_GETFL);

	if(tmp < 0) {
		//log("Err find ioctlsocket"," "," \n");
	}

	tmp = tmp | O_NONBLOCK;
	if(fcntl(fd, F_SETFL, tmp) < 0) {
		//log("Err set ioctlsocket"," "," \n");
	}
}

int cfdclose(int fd) {
	//char buf[16]="";
	
	//for (int i=0;i<1024;i++){
	//if (cfdIdx[Thread].Idx[i]==fd){
		close(fd);
		free((void *)(cfdIdx[fd].buf));
		cfdIdx[fd].buf=NULL;
		epoll_ctl(kepollFd, EPOLL_CTL_DEL, fd, NULL);
		//IP2str(buf,ipset[l].bind);
		//log("socket close,from listen ip ",buf," \n");
		return 0;
		//break;
//}}
			
	//return -1;
}

int main(int argc, char *argv[])
{
	if (argc<2){exit(0);}
	if (strcmp(argv[1],"-d")){
		fprintf(stderr,"command error \n");
		exit(0);
	}
	
	if (strlen(argv[2])<40){strcpy(ipset.bind,argv[2]);}
	if (strlen(argv[3])<7){ipset.bindPort=atoi(argv[3]);}
	if (strlen(argv[4])<16){strcpy(ipset.connect,argv[4]);}
	if (strlen(argv[5])<7){ipset.connectPort=atoi(argv[5]);}
	if (strlen(argv[6])<2){ipset.ipStatus=atoi(argv[6]);}
	if (strlen(argv[7])<16){strcpy(deny[0].ip,argv[7]);}
	if (strlen(argv[8])<16){strcpy(deny[1].ip,argv[8]);}
SIdx=(int *)(malloc(4*65536));
cfdIdx=(struct _cfdIdx *)(malloc(65536*sizeof(struct _cfdIdx)));

	printf("starting reforward...\n");
	int i, fuck_tcp, bindFd, cfd, tmpFd, ev;
	int readlen;
	int addrlen;

	struct epoll_event tmpEvent, keEvent[16384];

	if ((kepollFd = epoll_create(16384)) < 0)
			{
				fprintf(stderr,"Error: can't Create epoll Event \n");
				exit(-1);
			}

	struct sockaddr_in clientaddr;
	struct sockaddr_in6 clientaddr6;
	struct sockaddr_in serveraddr;
	struct sockaddr_in6 serveraddr6;
	if (ipset.ipStatus==4){bindFd = socket(AF_INET, SOCK_STREAM, 0);}
	if (ipset.ipStatus==6){bindFd = socket(AF_INET6, SOCK_STREAM, 0);}
	ioctlsocketX(bindFd);

	tmpEvent.data.fd = bindFd;
	tmpEvent.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(kepollFd, EPOLL_CTL_ADD, bindFd, &tmpEvent) < 0)
			{
				//log("Error: epoll can't listen fd"," "," \n");
				exit(-1);
			}
			
if (ipset.ipStatus==4){
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	char *local_addr = "127.0.0.1";
	inet_aton(ipset.bind, &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(ipset.bindPort);

	if (bind(bindFd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <0) 
			{
				
				fprintf(stderr, "Error: couldn't Bind to address %s port %d\n",ipset.bind, ipset.bindPort);
				close(bindFd);
				exit(-1);
			}
}

if (ipset.ipStatus==6){
	memset(&serveraddr6,0,sizeof(serveraddr6));
	serveraddr6.sin6_family = AF_INET6;
	char *local_addr = "::1";
	inet_pton(AF_INET6,ipset.bind, &(serveraddr6.sin6_addr));
	serveraddr6.sin6_port = htons(ipset.bindPort);

	if (bind(bindFd, (struct sockaddr *)&serveraddr6, sizeof(serveraddr6)) <0) 
			{
				
				fprintf(stderr, "Error: couldn't Bind to address %s port %d\n",ipset.bind, ipset.bindPort);
				close(bindFd);
				exit(-1);
			}
}
	if (listen(bindFd, 5) <0) {

				fprintf(stderr, "Error: couldn't listen to ip address %s port %d\n",ipset.bind, ipset.bindPort);	
				close(bindFd);
				exit(-1);
			}

	fuck_tcp = 1;
	char ipaddr[40];
	while(1) {
		ev = epoll_wait(kepollFd, keEvent, 16384, -1);
		
		for(i = 0; i < ev; ++i) {
			//if (fuck_tcp==1){
			if(keEvent[i].data.fd == bindFd) {
				//printf("accept connection, fd is %d\n", bindFd);
				cfd = accept(bindFd, (struct sockaddr *)&clientaddr6, &addrlen);
				if(cfd < 0) {
					printf("cfd < 0\n");
					continue;
				}
				
				inet_ntop(AF_INET6,&(clientaddr6.sin6_addr),&ipaddr,40);
				
				if (!strcmp(ipaddr,"::")) {
					memcpy(&clientaddr,&clientaddr6,16);
					strcpy(ipaddr,inet_ntoa(clientaddr.sin_addr));
				}
				
				printf("connect from %s\n", ipaddr);
				
				if (strlen(ipaddr)>8){
					if (ipaddr[0]==':' && ipaddr[1]==':' && ipaddr[2]=='f' && ipaddr[3]=='f' && ipaddr[4]=='f' && ipaddr[5]=='f' && ipaddr[6]==':') {
						char ipv4buf[16];
						memcpy(ipv4buf,ipaddr+7,16);
						strcpy(ipaddr,ipv4buf);
					}
				}
				
				if(cfd >60000) {
					printf("connect too much!\n");
					close(cfd);
					continue;
				}
				
				if (!strcmp(ipaddr,deny[0].ip)) {
					printf("deny connect ip %s\n", ipaddr);
					close(cfd);
					continue;
				}
				if (!strcmp(ipaddr,deny[1].ip)) {
					printf("deny connect ip %s\n", ipaddr);
					close(cfd);
					continue;
				}
				
				SIdx[cfd]=refdlink();
				if (SIdx[cfd]<0) {
					printf("connect cfd < 0 \n");
					close(cfd);
					continue;
				}
				
				SIdx[SIdx[cfd]]=cfd;
				
				ioctlsocketX(cfd);
				ioctlsocketX(SIdx[cfd]);
				
				cfdIdx[cfd].buf=(char *)(malloc(1536));
				cfdIdx[SIdx[cfd]].buf=(char *)(malloc(1536));
				
				tmpEvent.data.fd = cfd;
				tmpEvent.events = EPOLLIN | EPOLLET;
				epoll_ctl(kepollFd, EPOLL_CTL_ADD, cfd, &tmpEvent);
				
				tmpEvent.data.fd = SIdx[cfd];
				tmpEvent.events = EPOLLIN | EPOLLET;
				epoll_ctl(kepollFd, EPOLL_CTL_ADD, SIdx[cfd], &tmpEvent);
				continue;
			}
			
			
			if(keEvent[i].events & EPOLLIN) {
				if((tmpFd = keEvent[i].data.fd) < 0) continue;
				int n;
				if((n = recv(tmpFd, cfdIdx[tmpFd].buf, 1536,0)) < 0) {
					if(errno == ECONNRESET) {
						printf("ECONNRESET\n");
						cfdclose(tmpFd);
						cfdclose(SIdx[tmpFd]);
						keEvent[i].data.fd = -1;
					} else {
						if (errno == EWOULDBLOCK) {continue;}
						if (errno == EINPROGRESS) {continue;}
						cfdclose(tmpFd);
						cfdclose(SIdx[tmpFd]);
						keEvent[i].data.fd = -1;
					}
				continue;
				} else if(n == 0) {
					printf("close tmpFd\n"); //正常退出
					cfdclose(tmpFd);
					cfdclose(SIdx[tmpFd]);
					keEvent[i].data.fd = -1;
					continue;
				}
				
				cfdIdx[tmpFd].rlen=cfdIdx[tmpFd].len=n;
				tmpEvent.data.fd = tmpFd;
				tmpEvent.events = EPOLLOUT;
				epoll_ctl(kepollFd, EPOLL_CTL_MOD, tmpFd, &tmpEvent);
				continue;
			}
			
			if(keEvent[i].events & EPOLLOUT) {
				tmpFd = keEvent[i].data.fd;
				int len;
				len=send(SIdx[tmpFd], cfdIdx[tmpFd].buf+(cfdIdx[tmpFd].rlen-cfdIdx[tmpFd].len), cfdIdx[tmpFd].len,0);
				if (cfdIdx[tmpFd].len==len){
				tmpEvent.data.fd = tmpFd;
				tmpEvent.events = EPOLLIN;
				epoll_ctl(kepollFd, EPOLL_CTL_MOD, tmpFd, &tmpEvent);
				} else {
				if (len >0){cfdIdx[tmpFd].len-=len;} 
				if (len <0){
					if(errno == ECONNRESET) {
						printf("ECONNRESET\n");
						cfdclose(tmpFd);
						cfdclose(SIdx[tmpFd]);
						keEvent[i].data.fd = -1;
					} else {
						if (errno == EWOULDBLOCK) {continue;}
						if (errno == EINPROGRESS) {continue;}
						cfdclose(tmpFd);
						cfdclose(SIdx[tmpFd]);
						keEvent[i].data.fd = -1;
					}
				} else {
					//len=0;
					cfdclose(tmpFd);
					cfdclose(SIdx[tmpFd]);
					keEvent[i].data.fd = -1;
				}
				}
			}
		}
	}
}
