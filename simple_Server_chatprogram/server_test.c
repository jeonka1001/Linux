#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX_CLIENT (128)
int client_table[MAX_CLIENT]={0,};
int cnt=0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void broad_cast(int sock,char *msg,int len){
	pthread_mutex_lock(&mtx);
	for(int i = 0 ; i < cnt; i++){
		if(client_table[i] != sock){
			write(client_table[i],msg,len);
		}
	}
	pthread_mutex_unlock(&mtx);
}

void add_client(int sock){
	pthread_mutex_lock(&mtx);
	client_table[cnt++] = sock;
	pthread_mutex_unlock(&mtx);
}

void delete_client(int sock){
	pthread_mutex_lock(&mtx);
	for(int i = 0 ; i < cnt ; i ++){
		if(sock == client_table[i]){
			client_table[i] = client_table[--cnt];
		}
	}
	pthread_mutex_unlock(&mtx);
}

void *thread_main(void *arg){
	int csock = (int)arg;
	add_client(csock);
	while(1){
		char buf[BUFSIZ];
		int nRead = read(csock,buf,sizeof(buf));
		if(nRead<0){
			perror("read");
			break;
		}	
		else if( nRead == 0 ) {
			printf("No.%d disconnected .. \n",csock);
			break;
		}
		buf[nRead-1]=0;
		broad_cast(csock,buf,nRead);
	}
	delete_client(csock);
	close(csock);
	return NULL;
}


int main(){
	int ssock = socket(PF_INET,SOCK_STREAM,0);
	if(ssock == -1){
		perror("socket");
		return -1;
	}
	struct sockaddr_in saddr = {0,};
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8080);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int value = 1;
	if( setsockopt(ssock,SOL_SOCKET,SO_REUSEADDR,(char*)&value,sizeof(value))==-1){
			perror("setsockpot");
			return -1;
	}
	if(bind(ssock,(struct sockaddr*)&saddr,sizeof(saddr))==-1){
		perror("listen");
		return -1;
	}
	if(listen(ssock,10) ==-1){
		perror("listen");
		return -1;
	}
	printf("server is run .. \n" );
	while(1){
		struct sockaddr_in caddr={0,};
		unsigned int caddr_len = sizeof(caddr);
		int csock = accept(ssock,(struct sockaddr *)&caddr,&caddr_len);
		if(csock <0){
			perror("accept");
			return -1;
		}
		printf("[server]%s is connected ... \n ",inet_ntoa(caddr.sin_addr));
		pthread_t tid ;
		int ret = pthread_create(&tid,NULL,thread_main,(int*)csock);
		if(ret !=0){
			perror("pthread_create");
			return -1;
		}
		ret = pthread_detach(tid);
		if(ret != 0){
			perror("pthread_create");
			return -1;
		}
	}
	close(ssock);
	return 0;
}


		

