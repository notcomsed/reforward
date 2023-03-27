void *rNs(void *argv){
	int tmpFd;
	memcpy(&tmpFd,argv,4);
	while(1){
		int len = recv(tmpFd, cfdIdx[tmpFd].buf, 1536,0);
		pthread_testcancel();
		if(len <= 0) {
			_log("connect ","closed"," \n");
			cfdclose(tmpFd);
			pthread_cancel(worker[SIdx[tmpFd]]);
			cfdclose(SIdx[tmpFd]);
			regD[tmpFd]=0;
			regD[SIdx[tmpFd]]=0;
			pthread_exit(0);
			printf("error on exit\n");
			return NULL;
		}
				
		send(SIdx[tmpFd], cfdIdx[tmpFd].buf, len,0);
	}
	return NULL;	
}

