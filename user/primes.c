#include "kernel/types.h"
#include "user/stddef.h"
#include "user/user.h"


void primes(int fd[2]);

void main(){
	int fd[2];
	pipe(fd);
	if (fork()){
		close(fd[0]);
		for(int i=2;i<=35;i++){
			write(fd[1],&i,sizeof(int));	
		}
		close(fd[1]);
	}else{
		primes(fd);
	}
	wait(NULL);
	exit(0);
}

void primes(int lfd[2]){
	close(lfd[1]);
	int base,num,rfd[2];
	// first time read from left pipe. && if it is last child, exit fristly.
	if(read(lfd[0],&base,sizeof(int))==0){
		close(lfd[0]);
		exit(0);
	}
	printf("prime %d\n",base);
	pipe(rfd);

	if(fork()){
		int rs;
		do{
			rs=read(lfd[0],&num,sizeof(int));
			if (num%base!=0){
				write(rfd[1],&num,sizeof(int));
			}
		}while(rs);
		close(lfd[0]);
		close(rfd[1]);
	}else{
		primes(rfd);
	}
	wait(NULL);
	exit(0);
}
