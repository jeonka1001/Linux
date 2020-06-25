#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>


void* thread_main(void* arg){
	int csock = (int)arg;
	while(1){
		char buf[BUFSIZ];
		int nRead = read(csock,buf,sizeof(buf));
		if(nRead < 0 ){
			perror("read");
			return NULL;
		}
		else if(nRead == 0){
			return NULL;
		}
		buf[nRead-1]=0;
		printf("Other ] %s\n",buf);
	}
	return NULL;
}



int main(int argc, char**argv){
	if(argc != 2){
		fprintf(stderr,"usage : %s IPADDR\n",*argv);
		return -1;
	}
	--argc,++argv;

	int csock = socket(PF_INET,SOCK_STREAM,0); // 클라이언트 소켓 생성
	if( csock == -1){
		perror("socket");
		return -1;
	}
	
	struct sockaddr_in caddr={0,};
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(8080);
	caddr.sin_addr.s_addr = inet_addr(*argv); // 클라이언트는 전달인자로 주소를 받는다.

	if(connect(csock,(struct socaddr*)&caddr, sizeof(caddr)) ==-1){
		perror("connect");
		return -1;
	}
	pthread_t tid;
	pthread_create(&tid,NULL,thread_main,(void*)csock);
	system("clear");
	printf("서버와 연결 완료\n");
	while(1){
		char buf[BUFSIZ];
		int nRead = read(0,buf,sizeof(buf));
		if(nRead < 0 ){
			perror("read");
			return -1;
		}
		else if(nRead == 0 ){
			fprintf(stderr,"시스템을 종료 합니다.\n");
			write(csock,buf,sizeof(buf));
			close(csock);
			return 0;
		}
		buf[nRead-1]=0;
		write(csock,buf,sizeof(buf));
	}
	return 0;
}

