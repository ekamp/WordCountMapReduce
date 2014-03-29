// struct for the ut hashmap
struct
wordDictionary
{
	char* key; // using the word as the key
	int value; // number of occurrence of the word
	UT_hash_handle hh;	
}; typedef struct wordDictionary *wordDictionaryPtr;

// struct for sorting thread information
typedef struct 
{
	int id;
	char* file;
	wordDictionaryPtr wdptr;	
} _thread_id;

void
addWordReduce(char* word, wordDictionaryPtr* wdptr,int count);

void
addWord(char* word, wordDictionaryPtr* wdptr);

void
deleteGeneratedFiles(char* file);

void
free_uthash(wordDictionaryPtr wdptr);

void
mapFile(char* infile, int fileNum, wordDictionaryPtr *holder);

void*
mapperController(void *arg);

void
masterReduce(wordDictionaryPtr *master,wordDictionaryPtr *wdptr);

void
print_words(wordDictionaryPtr* wdptr);

void
readFile(char* name, wordDictionaryPtr* wdptr);

void
runTheMappers(int numberOfMappers, char* baseFileName, int numberOfReducers);

int
sortWord(wordDictionaryPtr ptr1, wordDictionaryPtr ptr2);

void
splitFile(char* fileName, char* numberOfPieces);

void
writeWordCount(char* file, wordDictionaryPtr* wdptr);

void
writeWordSort(char* file, wordDictionaryPtr* wdptr);

