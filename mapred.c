/*
 * @author: Hua Yang
 * @RUID: 128-00-2637
 * @author: Erik Kamp
 * @RUID: 132-00-4838
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include "uthash.h"
#include "mapred.h"

#define wordBufferSize 4096

/*
 * @param word : the desired word that one wishes to know the value of it is the ket and this will get the value
 */
wordDictionaryPtr global_wdptr = NULL;

void
addWordReduce(char* word, wordDictionaryPtr* wdptr,int count)
{
	wordDictionaryPtr tempNode = NULL;
	HASH_FIND_STR(*wdptr, word, tempNode);
    if (tempNode) {
    	tempNode->value+= count;
    } else {
    	wordDictionaryPtr addWord;
		addWord = malloc(sizeof(struct wordDictionary));
		addWord->key = (char*) calloc(strlen(word), sizeof(char));
		strcpy(addWord->key, word);
		addWord->value = 1;

		HASH_ADD_STR(*wdptr, key, addWord);   
    }
}

/*
 * Create a new hash struct with the given word,
 *   then insert into the main hash.
 * @param word : word to be added to the hash table
 */
void
addWord(char* word, wordDictionaryPtr* wdptr)
{
	wordDictionaryPtr tempNode = NULL;
	HASH_FIND_STR(*wdptr, word, tempNode);
    if (tempNode) {
    	tempNode->value++;
    } else {
    	wordDictionaryPtr addWord;
		addWord = malloc(sizeof(struct wordDictionary));
		addWord->key = (char*) calloc(strlen(word), sizeof(char));
		strcpy(addWord->key, word);
		addWord->value = 1;

		HASH_ADD_STR(*wdptr, key, addWord);   
    }
}

int
sortWord(wordDictionaryPtr ptr1, wordDictionaryPtr ptr2)
{
    return strcmp(ptr1->key, ptr2->key);
}

/*
 * @param fileName : the name of the file to be split into new files containing each 1000 orso to be read faster by the mappers
 * @param numberOfPieces : the number of pieces the file will be split into. This will coorespond to the number of mappers so that one mapper gets one piece
 */
void
deleteGeneratedFiles(char* file)
{
	int len = strlen(file) + 5;
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
	pid_t childLabor, lazyParent;
	int status = 0;
	childLabor = fork();
    if (childLabor == 0) {
    	char* splitFile[] = {"./split.sh", fileName, numberOfPieces, '\0'};

	    if (execvp("./split.sh",splitFile) < 0) {
	    	perror("./split.sh");
	    }
    } else if (childLabor < 0) {
    	perror("child");
    } else {
    	//int returnStatus;    
   		//waitpid(childLabor, &returnStatus, 0);
    }

    while ((lazyParent = wait(&status)) > 0) {
    	// wait for child to finish
    }
}

void
free_uthash(wordDictionaryPtr wdptr)
{
	wordDictionaryPtr ptr;

    for(ptr = wdptr; ptr != NULL; ptr = ptr->hh.next) {
    	HASH_DEL(wdptr, ptr);
    	free(ptr);
    }
}

void
print_words(wordDictionaryPtr* wdptr)
{
    wordDictionaryPtr s;

    for(s = *wdptr; s != NULL; s = s->hh.next) {
        printf("Key %s: Value %d\n", s->key, s->value);
    }
}

/*
 * This funciton reads in a file and prints it to the console taking in the name of the file as input.
 * @param name : String that contains the name of the input file
 */
void
readFile(char* name, wordDictionaryPtr* wdptr) 
{
	int c;
	FILE* fp;
	struct dirent *data;
	char *nextName,
		 *relPath,
		 *token,
		 *saveptr;
	char word[wordBufferSize];
	wordDictionaryPtr tempWDPtr;


	//printf("\nThe name of the file is |%s| \n",name);

	//Check to make sure the name is legal
	if(name == NULL || name[strlen(name)-1] == '.') {
		return;
	}

	//Make sure the file exists
	if ((fp = fopen(name, "r")) != NULL) {
		//Loop over the characters of the file in order to print them to the console
		while (fgets(word, wordBufferSize, fp) != NULL)  {
			token = strtok_r(word, " .,:;!?&$@|[]{}/\'\"*#-_<>()~\r\n",&saveptr);
			while (token) {
    			addWord(token, wdptr);
    			token = strtok_r(NULL, " .,:;!?&$@|[]{}/\'\"*#-_<>()~\r\n",&saveptr);
    		}
		}
		fclose(fp);
	} else {
		fprintf(stderr , "ERROR: %s is not a file or directory.\n", name);
	}
}

void
mapFile(char* infile, int fileNum, wordDictionaryPtr *holder)
{
	int len;
	char *file, *buffer;

	if (fileNum == 0)
		len = 1;
	else
		len = floor(log10(abs(fileNum))) + 1;		

	file = (char*) calloc(strlen(infile) + len + 2, sizeof(char));
	buffer = (char*) calloc(len, sizeof(char));
	sprintf(buffer, "%d", fileNum);
	strcat(file, infile);
	strcat(file, ".");
	strcat(file, buffer);
	strcat(file, "\0");

	readFile(file, holder);

	printf("%s.%d have %d words\n", infile, fileNum, HASH_COUNT(*holder));   
}

void*
mapperController(void *arg)
{
	_thread_id *p = (_thread_id *)arg;
	mapFile(p->file, p->id, &(p->wdptr) );
	return (NULL);
}

void
masterReduce(wordDictionaryPtr *master,wordDictionaryPtr *wdptr)
{
	wordDictionaryPtr s;

    for(s = *wdptr; s != NULL; s = s->hh.next) {
    	addWordReduce(s->key, master, s->value);
    }
}

/* 
 *	This function will launch all the mappers, and the mappers as specified by the user each one getting a certain setion of the input file
 *	@param numberOfMappers : number of mappers to be run at the same time
 *	@param baseFileName : the base name of the file that was split via the split command
 *	@param numberOfReducers : the number of reduces to be run at one time
 */
void
runTheMappers(int numberOfMappers, char* baseFileName, int numberOfReducers)
{
	int i, r;
	pthread_t *threads;
	pthread_attr_t pthread_custom_attr;
	_thread_id *p;

	threads = (pthread_t *) malloc(numberOfMappers*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

	p = (_thread_id *) malloc(sizeof(_thread_id)*numberOfMappers);
	/* Start up thread */

	for (i = 0; i < numberOfMappers; i++) {
		p[i].id = i;
		p[i].file = (char*) calloc(strlen(baseFileName), sizeof(char));
		strcpy(p[i].file, baseFileName);
		p[i].wdptr = NULL;
		pthread_create(&threads[i], &pthread_custom_attr, mapperController, (void *)(p+i));
	}

	/* Synchronize the completion of each thread. */
	for (i = 0; i < numberOfMappers; i++) {
		pthread_join(threads[i],NULL);
	}
	
	for (i = 0; i < numberOfMappers; i++) {
		masterReduce(&global_wdptr,&(p[i].wdptr)); 
	}

	for (i = 0; i < numberOfMappers; i++) {
		free_uthash(p[i].wdptr);
		free(p[i].file);
	}

}

void
writeWordCount(char* file, wordDictionaryPtr* wdptr)
{
	FILE* fp;
	wordDictionaryPtr ptr;

	if ((fp = fopen(file, "w+")) != NULL) {
	    for(ptr = *wdptr; ptr != NULL; ptr = ptr->hh.next)
			fprintf(fp, "%s : %d\n", ptr->key, ptr->value);
	}

	fclose(fp);
}

void
writeWordSort(char* file, wordDictionaryPtr* wdptr)
{
	FILE* fp;
	wordDictionaryPtr ptr;

	if ((fp = fopen(file, "w+")) != NULL) {
		HASH_SORT(*wdptr, sortWord);

	    for(ptr = *wdptr; ptr != NULL; ptr = ptr->hh.next)
	        fprintf(fp, "%s\n", ptr->key);
	}

	fclose(fp);
}

int
main(int argc, char** argv)
{
	FILE *input,
		 *output;
	char safety,
		 tooMany;
	char *typeOfRun,
		 *threadOrProc,
		 *infile,
		 *outfile,
		 *numberOfMappers;
	int numberOfReducers;

	// assign the variables from the input taken from the command line args
	typeOfRun = argv[2];
	threadOrProc = argv[4];
	numberOfMappers = argv[6];
	numberOfReducers = atoi(argv[8]);
	infile = argv[9];
	outfile = argv[10];

	// if incorrect number of arguments
	if (argc != 11) {
		fprintf(stderr, "ERROR: Incorrect number of arguments\n");
		return 0;
	}

	if (strcmp(threadOrProc, "threads") != 0) {
		fprintf(stderr, "The program will only run threads!\nEnding program.\n");
		return 0;
	}

	// disallow overwiting our source files in the output
	if (strcmp(outfile,"mapred.o") == 0 ||
		strcmp(outfile,"Makefile") == 0 ||
		strcmp(outfile,"readme.pdf") == 0) {
		fprintf(stderr, "Please don't try to overwrite our sourcefiles with the output\n");
		return 0;
	}

	if ((input = fopen(infile, "r")) == NULL) {
		fclose(input);
		fprintf(stderr, "The input file, %s, does not exist!\nEnding program.\n", infile);
		return 0;
	} else {
		fclose(input);
	}

	if ((output = fopen(outfile, "r")) != NULL) {
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
	// run the mappers
	runTheMappers(atoi(numberOfMappers), infile, numberOfReducers);
	// check if output in word count format or sort format
	if (strcmp(typeOfRun, "sort") == 0) {
		writeWordSort(outfile, &global_wdptr);
	} else {
		writeWordCount(outfile, &global_wdptr);
	}

	fclose(output);
	deleteGeneratedFiles(infile);
	return 0;
}
