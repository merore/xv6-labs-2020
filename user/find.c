#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char* fmtname(char* path){
    char* p;
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char* path, char* filename){
    int fd;
    struct stat st;
    struct dirent de;
    if((fd = open(path,0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd,&st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
	if(strcmp(fmtname(path),filename) == 0){
		printf("%s\n",path);
	}
	if(st.type == T_DIR){
		while(read(fd,&de,sizeof(de)) == sizeof(de)){
			if(de.inum == 0){
				break;
			}
			char* subPath = (char*)malloc(strlen(path)+strlen(de.name)+2);
			memmove(subPath,path,strlen(path));
			memmove(subPath+strlen(path),"/",1);
			memmove(subPath+strlen(path)+1,de.name,strlen(de.name)+1);
			stat(subPath,&st);
			switch(st.type){
			case T_FILE:
				 if(strcmp(de.name,filename) == 0){
				     printf("%s\n",subPath);
				 }
				 break;
			case T_DIR:
				 if(strcmp(de.name,".")!=0 && strcmp(de.name,"..")!=0){
					 find(subPath,filename);
				 }
				 break;
			}
		}
	}
    close(fd);
}

void main(int argc,char* argv[]){
	if(argc !=3){
		fprintf(2,"find: need 2 arguments\n");
		exit(0);
	}
	find(argv[1],argv[2]);
	exit(0);
}
