#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

/*
@param : name
	String that contains the name of the input file

This funciton reads in a file and prints it to the console taking in the name of the file as input
*/
void readFile(char* name){
	int c;
	FILE * fileName;
	struct dirent *data;
	char * nextName;
	char * relPath;

	//Check to make sure the name is legal
	if(name==NULL||name[strlen(name)-1]=='.')
	{
		return;
	}
	//Make sure the file exists
	if ((fileName = fopen(name,"r")) != NULL)
	{
			//Loop over the characters of the file in order to print them to the console
			while ((c = getc(fileName)) != EOF)
        		printf("%c",c);
        	//Close the file after reading
			fclose(fileName);
	}
	else
	{
		fprintf(stderr , "ERROR: %s is not a file or directory.\n", name);
		return;
	}
}



int main(int argc, char **argv)
{
	FILE *output;
	char safety;
	char tooMany;

	/*If incorrect number of arguments */
	if (argc != 3)
	{
		fprintf(stderr, "ERROR: Incorrect number of arguments\n");
		return 0;
	}

	/*disallow overwiting our source files in the output*/
	if (strcmp(argv[1],"libws.a")==0||strcmp(argv[1],"wordstat.o")==0||strcmp(argv[1],"index")==0||strcmp(argv[1],"indexer.c")==0||strcmp(argv[1],"indexer.h")==0||strcmp(argv[1],"Makefile")==0||strcmp(argv[1],"readme.pdf")==0||strcmp(argv[1],"testplan.txt")==0||strcmp(argv[1],"wordstat.c")==0||strcmp(argv[1],"wordstat.h")==0){
		fprintf(stderr, "Please don't try to overwrite our sourcefiles with the output\n");
		return 0;
	}

	if ((output=fopen(argv[1],"r"))!=NULL){
		fclose(output);
		fprintf(stdout, "The file designated for output already exists, are you sure you wish to overwite it? (y/n)");
		if(fscanf(stdin, "%c%c", &safety, &tooMany)!=2||safety!='y'||tooMany!='\n'){
			fprintf(stdout, "Ending program.\n");
			return 0;
		}
	}
	
	//Set the out file as the first argument from the commandline
	output = fopen(argv[1],"w");
	//Print the file specified by the second argument in the command line
	readFile(argv[2]);

	/*print to file here*/


	/*print and free*/
	//print(head,output);
	//freecharnode(head);
	fclose(output);
	return 0;
}


