#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include "uthash.h"

/*
 * @author: Hua Yang
 * @RUID: 128-00-2637
 * @author: Erik Kamp
 * @RUID: 132-00-4838
 * @author: Sharlina Keshava
 * @RUID: 140-00-9007
 */

//Initializing the struct to be used for the hashmap
struct wordDictionary
{
	char * word; //Using this as the key, this will be the word itself
	int value;
	UT_hash_handle hashHandle;	
};typedef struct wordDictionary *wordDictionaryPtr;

/*
	@param word : the desired word that one wishes to know the value of it is the ket and this will get the value
*/

wordDictionaryPtr *dictionaryPointer = NULL;

	wordDictionaryPtr findWord(char * word)
	{
		wordDictionaryPtr *myDiction;
		HASH_FIND_STR(dictionaryPointer,&word,myDiction);
		return myDiction;
	}

/*
 *	@param addedWord : word to be added to the hash table
 */

	void addWordOccurance(wordDictionaryPtr *addedWord)
	{
		HASH_ADD_STR(dictionaryPointer,addedWord->word,addedWord);
	}

/*
	@param fileName : the name of the file to be split into new files containing each 1000 orso to be read faster by the mappers
*/

void splitFile(char * fileName)
{
	char *splitFile[]={"split",fileName,"FILE:"};
	execvp("split",splitFile);
}


/*
@param name : String that contains the name of the input file

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
	// Before reading in the file make sure to split, the file in 25 or less parts, in his example he uses 25 soooo im just going to use it for now
	splitFile(argv[2]);
	//Print the file specified by the second argument in the command line
	readFile(argv[2]);

	fclose(output);
	return 0;
}
