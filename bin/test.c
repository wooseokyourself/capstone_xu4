#include <stdio.h>

int main(){
	FILE * fp;
	fp = fopen("test.txt", "w");
	fputs("test success\n", fp);
	fclose(fp);
	return 0;
}
