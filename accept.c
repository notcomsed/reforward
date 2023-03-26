void SecAccept(void *argc){
		int cfd;
		struct sockaddr_in clientaddr;
		struct sockaddr_in6 clientaddr6;
		const int addrlen=sizeof(clientaddr6);
		struct epoll_event tmpEvent;
		char ipaddr[40];
	while(1){
		//printf("accept connection, fd is %d\n", bindFd);
		memset(&clientaddr6,0,sizeof(struct sockaddr_in6));
		cfd = accept(bindFd, (struct sockaddr *)&clientaddr6, &addrlen);
		
				if(cfd < 0) {
					_log("accept connection cfd ","<0"," \n");
					break;
				}
				
				inet_ntop(AF_INET6,&(clientaddr6.sin6_addr),&ipaddr,40);
				
				if (!strcmp(ipaddr,"::")) {
					memcpy(&clientaddr,&clientaddr6,16);
					strcpy(ipaddr,inet_ntoa(clientaddr.sin_addr));
				}
				
				_log("connect from ",ipaddr, " \n");
				
				if (strlen(ipaddr)>8){
					if (ipaddr[0]==':' && ipaddr[1]==':' && ipaddr[2]=='f' && ipaddr[3]=='f' && ipaddr[4]=='f' && ipaddr[5]=='f' && ipaddr[6]==':') {
						char ipv4buf[16];
						memcpy(ipv4buf,ipaddr+7,16);
						strcpy(ipaddr,ipv4buf);
					}
				}
				
				if(cfd >60000) {
					_log("connect ","too"," much!\n");
					close(cfd);
					break;
				}
				
				if (!strcmp(ipaddr,deny[0].ip)) {
					_log("deny connect ip ",ipaddr," \n");
					close(cfd);
					break;
				}
				if (!strcmp(ipaddr,deny[1].ip)) {
					_log("deny connect ip ",ipaddr," \n");
					close(cfd);
					break;
				}
				
				SIdx[cfd]=refdlink();
				if (SIdx[cfd]<0) {
					_log("connect remote host ","<0"," \n");
					close(cfd);
					break;
				}
				
				SIdx[SIdx[cfd]]=cfd;
				
				cfdIdx[cfd].buf=(char *)(malloc(1536));
				cfdIdx[SIdx[cfd]].buf=(char *)(malloc(1536));
				
				regD[cfd]=0;
				regD[SIdx[cfd]]=0;
				tmpEvent.data.fd = cfd;
				tmpEvent.events = EPOLLIN | EPOLLET;
				epoll_ctl(kepollFd, EPOLL_CTL_ADD, cfd, &tmpEvent);
				
				tmpEvent.data.fd = SIdx[cfd];
				tmpEvent.events = EPOLLIN | EPOLLET;
				epoll_ctl(kepollFd, EPOLL_CTL_ADD, SIdx[cfd], &tmpEvent);
				break;
	}
}

