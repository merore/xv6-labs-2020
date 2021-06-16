#include "kernel/types.h"
#include "user/user.h"

void main(int argc, char* argv[]){
	if(argc<=1){
		fprintf(2,"sleep need one argument\n");
		exit(1);
	}
	sleep(atoi(argv[1]));
	exit(0);
}
