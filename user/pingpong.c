#include "kernel/types.h"
#include "user/user.h"

void main(){
	int fd[2];
	char s='a';
	pipe(fd);
	int pid=fork();

	if(0==pid){
		read(fd[0],&s,1);
		fprintf(1,"%d: received ping\n",getpid());
		write(fd[1],&s,1);
		exit(0);
	}else{
		write(fd[1],&s,1);
		wait((void*)0);
		read(fd[0],&s,1);
		fprintf(1,"%d: received pong\n",getpid());
		exit(0);
	}
}
