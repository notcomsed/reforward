

void helpf(){
	fprintf(stdout, "reforward\n");
	fprintf(stdout, "  -d, --direct start\n");
	fprintf(stdout, "  -v, --version\n");
	fprintf(stdout, "  -h, --help\n");
	fprintf(stdout, "reforward -d [listen] [listen Port] [connect] [connect Port] [IP version] [deny IP 1] [deny IP 2]\n");
	fprintf(stdout, "Examples:\n");
	fprintf(stdout, "reforward -d 0.0.0.0 8080 192.168.1.1 80 4 0 0\n"); 
	fprintf(stdout, "\"[deny IP 1]\"=\"0\" mean NOT deny\n\n"); 
	fprintf(stdout, "Deny examples:\n");
	fprintf(stdout, "reforward -d 0.0.0.0 8080 192.168.1.1 80 4 192.168.1.1 192.168.1.2\n"); 
	fprintf(stdout, "\"[deny IP 1]\"=\"192.168.1.1\" And \"[deny IP 2]\"=\"192.168.1.2\", deny 192.168.1.1 And 192.168.1.2\n\n");
	fprintf(stdout, "Use IPv6:\n");
	fprintf(stdout, "reforward -d :: 8080 192.168.1.1 80 6 0 0\n");
	fprintf(stdout, "reforward -d fd20::1 8080 192.168.1.1 80 6 0 0\n");
	exit(0); 
}

int main(int argc, char *argv[])
{
	if (argc<2){exit(0);}
	
	if (!strcmp(argv[1],"-h")){
		helpf();
	}
	
	if (!strcmp(argv[1],"--h")){
		helpf();
	}
	
	if (!strcmp(argv[1],"-v")){
		fprintf(stdout,"reforward v3.4.6\n");
		exit(0);
	}
	
	if (strcmp(argv[1],"-d")){
		fprintf(stderr,"command error \n");
		exit(0);
	}
	
	if (argc<=8){
		fprintf(stderr,"Segmentation fault\n");
		exit(-1);
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
	regD=(char *)(malloc(65536));
	worker=(pthread_t *)(malloc(65536*sizeof(pthread_t)));
	printf("start ....\n");

	struct epoll_event  keEvent[16384];
	if ((kepollFd = epoll_create(16384)) < 0)
			{
				fprintf(stderr,"Error: can't Create epoll Event \n");
				exit(-1);
			}
	if (initbindFd() <0 ){exit(-1);}
	
	if (1){
	struct epoll_event tmpEvent;
	tmpEvent.data.fd = bindFd;
	tmpEvent.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(kepollFd, EPOLL_CTL_ADD, bindFd, &tmpEvent) < 0)
			{
				printf("Error: epoll can't listen bindfd \n");
				exit(-1);
			}
	}	
	
	int tmpFd,space,ev;
	while(1) {
		ev = epoll_wait(kepollFd, keEvent, 16384, -1);
		
		for(int i = 0; i < ev; i++) {
			
			if(keEvent[i].events & EPOLLIN) {
				if((tmpFd = keEvent[i].data.fd) < 0) continue;
				if(keEvent[i].data.fd == bindFd) {		
				SecAccept(NULL);
				continue;
				}
				
				//printf("connectED, fd is %d,reg_status=%d\n", tmpFd,regD[tmpFd]);
				if (regD[tmpFd]==0){
					pthread_create(&worker[tmpFd],NULL, rNs, &tmpFd);
					regD[tmpFd]=1;
				if (regD[SIdx[tmpFd]]==0){
					pthread_create(&worker[SIdx[tmpFd]],NULL, rNs, &SIdx[tmpFd]);
					regD[SIdx[tmpFd]]=1;	
				}
				
				}
			}
		}
	}
}
