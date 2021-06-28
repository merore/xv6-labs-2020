#include "kernel/types.h"
#include "user/user.h"
#define MAXINPUT 256

void main(int argc, char* argv[]){
	char* buf=malloc(MAXINPUT);
    int n = read(0,buf,MAXINPUT);
    if(n>=MAXINPUT){
        fprintf(2, "xargs: out of max buf 256\n");
        exit(1);
    }

	char** nv=malloc(MAXINPUT);
	for(int i=0;i<argc-1;i++){
		nv[i]=argv[i+1];
	}

	int ni=0;
	int be=0;

	int i=0;
	while(i<n){
		switch(buf[i]){
		case ' ':
			buf[i]='\0';
			nv[argc-1+ni]=&buf[be];
			be=i+1;
			ni++;
			break;
		case '\n':
			buf[i]='\0';
			nv[argc-1+ni]=&buf[be];
			nv[argc+ni]='\0';
			be=i+1;
			ni=0;
			if(fork()==0){
				exec(argv[1],nv);
				exit(0);
			}
			wait((void*)0);
			break;
		}
		i++;
	}
	if(ni!=0){
		nv[argc+ni]='\0';
		for(int i=0;i<strlen((char*)nv);i++){
			printf("argv%d is %s\n",i,argv[i]);
		}
		if(fork()==0){
			exec(argv[1],nv);
			exit(0);
		}
	}
	wait((void*)0);
    exit(0);
}
