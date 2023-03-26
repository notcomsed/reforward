char initbindFd(){
	struct sockaddr_in serveraddr;
	struct sockaddr_in6 serveraddr6;
	
	if (ipset.ipStatus==4){bindFd = socket(AF_INET, SOCK_STREAM, 0);}
	if (ipset.ipStatus==6){bindFd = socket(AF_INET6, SOCK_STREAM, 0);}
	
	if (bindFd < 0) {
		fprintf(stderr, "couldn't create "
			"server socket!\n");
		bindFd = -1;
		return -1;
	}

if (ipset.ipStatus==4){
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_aton(ipset.bind, &(serveraddr.sin_addr));
	serveraddr.sin_port = htons(ipset.bindPort);

	if (bind(bindFd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) <0) 
			{
				
				fprintf(stderr, "Error: couldn't Bind to address %s port %d\n",ipset.bind, ipset.bindPort);
				close(bindFd);
				return -1;
			}
}

if (ipset.ipStatus==6){
	memset(&serveraddr6,0,sizeof(serveraddr6));
	serveraddr6.sin6_family = AF_INET6;
	inet_pton(AF_INET6,ipset.bind, &(serveraddr6.sin6_addr));
	serveraddr6.sin6_port = htons(ipset.bindPort);

	if (bind(bindFd, (struct sockaddr *)&serveraddr6, sizeof(serveraddr6)) <0) 
			{
				
				fprintf(stderr, "Error: couldn't Bind to address %s port %d\n",ipset.bind, ipset.bindPort);
				close(bindFd);
				return -1;
			}
}


	if (listen(bindFd, 5) <0) {

				fprintf(stderr, "Error: couldn't listen to ip address %s port %d\n",ipset.bind, ipset.bindPort);	
				close(bindFd);
				return -1;
			}
			
	int io=1;		
	ioctl(bindFd, FIONBIO, &io);		
	return 0;
}

