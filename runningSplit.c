#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>


int main(int argc, char **argv)
{


	printf("The file name should be %s",argv[1]);

	char *splitFile[]={"split",argv[1]};
	execvp("split",splitFile);

}