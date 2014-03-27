/*
 * @author: Hua Yang
 * @RUID: 128-00-2637
 * @author: Erik Kamp
 * @RUID: 132-00-4838
 * @author: Sharlina Keshava
 * @RUID: 140-00-9007
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include "uthash.h"

// Initializing the struct to be used for the hashmap
struct
wordDictionary
{
	char* key; // using the word as the key
	int value; // number of occurrence of the word
	UT_hash_handle hashHandle;	
}; typedef struct wordDictionary *wordDictionaryPtr;

/*
 * @param word : the desired word that one wishes to know the value of it is the ket and this will get the value
 */
wordDictionaryPtr dictionaryPointer = NULL;

wordDictionaryPtr findWord(char* word)
{
	wordDictionaryPtr ptr;
	HASH_FIND_STR(dictionaryPointer, &word, ptr);
	return ptr;
}

/*
 * Create a new hash struct with the given word,
 *   then insert into the main hash.
 * @param word : word to be added to the hash table
 */
void
addWord(char* word)
{
	wordDictionaryPtr addWord;
	addWord = malloc(sizeof(struct wordDictionary));
	addWord->key = (char*) calloc(strlen(word), sizeof(char));
	strcpy(addWord->key, word);
	addWord->value = 1;

	HASH_ADD_STR(dictionaryPointer, key, addedWord);
}

void

/*
 * @param fileName : the name of the file to be split into new files containing each 1000 orso to be read faster by the mappers
 * @param numberOfPieces : the number of pieces the file will be split into. This will coorespond to the number of mappers so that one mapper gets one piece
 */
void
deleteGeneratedFiles(char* file)
{
	int len = strlen(file)+5;
	char* cmd = (char*) calloc(len, sizeof(char));
	strcat(cmd, "rm ");
	strcat(cmd, file);
	strcat(cmd, ".*");
	cmd[len] = '\0';
	system(cmd);
}

void
splitFile(char* fileName, char* numberOfPieces)
{
	pid_t childLabor;
	childLabor = fork();
    if (childLabor == 0) {
    	char* splitFile[] = {"./split.sh", fileName, numberOfPieces, '\0'};

	    if (execvp("./split.sh",splitFile) < 0) {
	    	perror("./split.sh");
	    }
    } else if (childLabor < 0) {
    	perror("child");
    } else {
    	int returnStatus;    
   		waitpid(childLabor, &returnStatus, 0);
    }
}


/*
 * This funciton reads in a file and prints it to the console taking in the name of the file as input.
 * @param name : String that contains the name of the input file
 */
void
readFile(char* name) 
{
	int c;
	FILE * fileName;
	struct dirent *data;
	char * nextName;
	char * relPath;

	//Check to make sure the name is legal
	if(name==NULL||name[strlen(name)-1]=='.') {
		return;
	}
	//Make sure the file exists
	if ((fileName = fopen(name,"r")) != NULL) {
			//Loop over the characters of the file in order to print them to the console
			while ((c = getc(fileName)) != EOF)
        		printf("%c",c);
        	//Close the file after reading
			fclose(fileName);
	}
	else {
		fprintf(stderr , "ERROR: %s is not a file or directory.\n", name);
		return;
	}
}

int
main(int argc, char** argv)
{
	FILE *output;
	char safety;
	char tooMany;

	char * typeOfRun; 
	char * threadOrProc;
	char * infile;
	char * outfile;
	char * numberOfMappers;
	int numberOfReducers;

	// inputs from the command line are as follows (11 inputs including the .o file)
	//-a [wordcount,sort]   : states whether the user wants to use the wordcount mapred or int sort mapred (typeOfRun)
	//-i [procs,threads]    : states whether the user wishes to use processes or threads (threadOrProc)
	//-m num_maps 			: provides the number of mappers for the program (numberOfMappers)
	//-r num_reducers		: provides the number of reducers for the program (numberOfReducers)
	//infile 				: the file to be read in by the program (infile)
	//outfile				: the file to be writen to by the program (outfile)

	// assign the variables from the input taken from the command line args
	typeOfRun = argv[2];
	threadOrProc = argv[4];
	numberOfMappers = argv[6];
	numberOfReducers = atoi(argv[8]);
	infile = argv[9];
	outfile = argv[10];

	printf("Type of run is : %s\nWe are running on : %s\nThe number of mappers we have is : %s\nThe number of reducers is : %i\nThe input file name is : %s\nThe output file name is : %s\nThe number of args we have is : %i\n",
		typeOfRun,threadOrProc,numberOfMappers,numberOfReducers,infile,outfile,argc);

	// if incorrect number of arguments
	if (argc != 11) {
		fprintf(stderr, "ERROR: Incorrect number of arguments\n");
		return 0;
	}

	// disallow overwiting our source files in the output
	if (strcmp(outfile,"mapred.o") == 0 ||
		strcmp(outfile,"Makefile") == 0 ||
		strcmp(outfile,"readme.pdf") == 0) {
		fprintf(stderr, "Please don't try to overwrite our sourcefiles with the output\n");
		return 0;
	}

	if ((output = fopen(outfile,"r")) != NULL) {
		fclose(output);
		fprintf(stdout, "The file designated for output already exists, are you sure you wish to overwite it? (y/n): ");
		if(fscanf(stdin, "%c%c", &safety, &tooMany)!=2||safety!='y'||tooMany!='\n') {
			fprintf(stdout, "Ending program.\n");
			return 0;
		}
	}
	
	// set the out file as the first argument from the commandline
	output = fopen(outfile,"w");
	// before reading in the file make sure to split, the file in 25 or less parts, in his example he uses 25 soooo im just going to use it for now
	splitFile(infile,numberOfMappers);

	// print the file specified by the second argument in the command line uncomment this if you want to print the file
	//readFile(infile);
	fclose(output);

	deleteGeneratedFiles(infile);
	return 0;
}
