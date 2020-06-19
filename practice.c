// $ ./a.out a.txt
// -rw-r--r-- 1 linux linux 144 jun 17 23:59 a.txt

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>

void mylink(nlink_t st_link){ printf(" %hu ", st_link); return;}

void perm(mode_t st_mode){ // 파일의 권한을 출력하는 함수
	char perm[] = "----------";
	char a[]="rwx";
	for(int i = 0 ; i < 9 ; i ++){
		int aa = (st_mode >> (8-i)) & 0x01 ;
		perm[i+1]=(aa==1?a[i%3]:'-');
	}
	printf(" %s ",perm);
	return ;
}
void owner(const struct stat sbp){ // 파일의 소유자를 출력하는 함수
	struct passwd *pswd = getpwuid(sbp.st_uid); // 파일의 상태 객체를 통해 유저 아이디를 얻어온다
	if(pswd == NULL) {
		perror("getpwuid");
		return ;
	}

	struct group *grp = getgrgid(sbp.st_gid); // 파일의 상티 객체를 통해 그룹 아이디를 얻어온다.
	if(grp == NULL){
		perror("getgrgid");
		return ;
	}
	printf(" %s %s ",pswd->pw_name,grp->gr_name);
	return ;
}
void fsize(struct stat sb){
	if( S_ISBLK(sb.st_mode) || S_ISCHR(sb.st_mode)){ // 파일이 디바이스 파일이면 분류번호를 출력한다. 
		printf(" %d, %d ",(sb.st_rdev >> 8 ) &0xFF, sb.st_rdev );
	}else{
		printf(" %lld ",sb.st_size); // 일반 파일일 경우 파일의 크기 출력
	}
	return;
}

void mydate(struct stat sb){ // 파일 최종 수정시간 출력 
	char mtime[32]={0,};
	struct tm *t = localtime(&sb.st_mtime);
	strftime(mtime,sizeof(mtime),"%b, %e %R",t);
	printf(" %s ",mtime);
}

int main(int argc,char **argv){
	if(argc != 2) {
		fprintf(stderr,"usage : %s FILE \n" , *argv);
		return -1;
	}
	--argc, ++argv;
	struct stat sb ;
	if( lstat(*argv,&sb) == -1){
		perror("lstat");
		return -1; 
	}
	perm(sb.st_mode);
	mylink(sb.st_nlink);
	owner(sb);
	fsize(sb);
	mydate(sb);
	printf(" %s\n",*argv);
	return 0;
}

