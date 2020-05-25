#include <stdio.h>
#include <stdlib.h>	//exit()
#include <unistd.h>	//pipe()

#define BUFSIZE 30

int main(){
	FILE *fp = fopen("clnt_output.txt", "w");
	int fd[2];
	char buffer[BUFSIZE];
	if(pipe(fd) ==  -1){
		fputs("pipe error\n", fp);
		exit(1);	
	}
	fputs("HHI", fp);
	fclose(fp);
	read(fd[0], buffer, BUFSIZE);
	//printf("%s\n", buffer);
	return 0;
}
