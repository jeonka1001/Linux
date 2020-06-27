#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>

int main (){
	int csock = socket(PF_INET,SOCK_STREAM,0);
	if(csock < 0 ){
		perror("socket");
		return -1;
	}

	struct sockaddr_in caddr;
	size_t caddr_len = sizeof(caddr);
	memset(&caddr,0,caddr_len);
	caddr.sin_port = 8080;
	caddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	caddr.sin_family = AF_INET;

	if(connect(csock,(struct sockaddr*)&caddr , sizeof(caddr)) == -1 ){
		perror("connect");
		return -1;
	}

	char *num;
	int nRead = read(0,num,sizeof(num));
	if(nRead < 0 ) { 
		perror("read");
		return -1;
	}
	int argc = atoi(num);
	printf("argc = %d\n",argc);
	write(csock,num,sizeof(num));
	int argv[argc+1];
	size_t argv_len = sizeof(argv[0]);
	for( int i = 0 ; i < argc ; i ++){
		int nRead = read(0,&argv[i],argv_len);
		if(nRead < 0){
			perror("read");
			return -1;
		}
		else if (nRead == 0){
			break;
		}
		write(csock,&argv[i],argv_len);
	}
	for(int i = 0 ; i < argc ; i ++ ){
		printf(" %d ",argv[i]);
	}
	printf("\n");
	char operator[argc];
	size_t operator_len = sizeof(operator)/sizeof(operator[0]);
	for(int i = 0 ; i < argc -1 ; i ++){
		int nRead = read(csock,&operator[i],operator_len);
		if(nRead < 0 ) {
			perror("read");
			return -1;
		}
		else if(nRead == 0 ){
			break;
		}
		write(csock,&operator[i],operator_len);
	}
	for(int i = 0 ; i < argc ; i ++ ){
		printf(" %d ",operator[i]);
	}
	printf("\n");
	int answer;
	nRead = read(csock,&answer,sizeof(answer));
	printf("answer : %d",answer);
	return 0;
}
