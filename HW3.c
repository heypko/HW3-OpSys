#if 0

	Peter Ko
	Operating Systems
	HW1 - C Strings and Dynamic Allocation of Memory

#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>


// Create struct to manage words and their respective filenames.
typedef struct WFile WFile;
struct WFile {
	char* file;
	char* word;
};

		                              
void * threadFun() {
	fprintf(stdout, "Butts\n");
	return NULL;
}

int main(int argc, char *argv[]) {
	
	/* check to see if a single file was used */
	if (argc != 3) { 
		fprintf( stderr, "ERROR, invalid number of arguments.\n");
		return EXIT_FAILURE;
	}

	/* Initialize counter for individual word length, number counters, character string */
	int wordLength = 0;
	int wordCount = 0;
	int maxCount = 8;


	char* findWord = argv[2];

	/* Create and allocate space for array of POINTERS(calloc)*/
	WFile **wordArray = (WFile **)calloc(8, sizeof(WFile*));
	fprintf( stdout, "MAIN THREAD: Allocated initial array of 8 character pointers.\n");


	/* Open directory */
	DIR *rootdir;
	struct dirent *entries;
	int fileCount = 0;

	rootdir = opendir(argv[1]);

	if (rootdir != NULL)
	{
		while ((entries = readdir(rootdir)) != NULL) {
			//puts(entries->d_name);
			if (entries->d_name[0] == '.') {
				continue;
			}
			else {
				++fileCount;
				fprintf ( stdout, "In Directory: %s\n", entries->d_name);
			}
		}
		fprintf( stdout, "FileCount = %d\n", fileCount);
		closedir(rootdir);
	}

	else {
		fprintf( stderr, "MAIN THREAD: Could not open directory. Exiting Program.\n");
		return EXIT_FAILURE;
	}

	pthread_t tid[ fileCount ];   /* keep track of the thread IDs */
  	int i, rc;
  	

  	/* Allocate thread space */
  	rewinddir(rootdir);
  	
	/* create the threads */
	for ( i = 1 ; i <= fileCount; i++ )
	{

		// 
		printf( "MAIN: Assigned \"%s\" to child thread %d.\n", entries->d_name, rc);
		fprintf(stderr, "Hihihi\n");
		rc = pthread_create( &tid[i], NULL, threadFun, NULL );


		if ( rc != 0 ) {
		  fprintf( stderr, "MAIN: Could not create child thread (%d)\n", rc );
		}
	}


		

	pthread_t pthread = pthread_self(); /* thread id */


	// // Create & Assign threads to multiple files in single directory
	// /* create pipes */ 
	// int pipeCount;
	// int **pipeArray = malloc(fileCount*sizeof(int*));
	// for (pipeCount = 0; pipeCount < fileCount - 1; ++pipeCount) {
	// 	pipeArray[pipeCount] = malloc(2*sizeof(int));
	// }





	/* Create file pointer to read text */
	FILE *filePointer;

	char* fileName = argv[1]; // This could be problematic in the future

	int fileNameLen = strlen(fileName);
	//fprintf(stderr, "%d\n", fileNameLen);
	filePointer = fopen(argv[1], "r");

	/* Start loop iterating through file */
	while (!feof(filePointer)) {
	
		/* Re-allocate space for array of POINTERS (realloc) if necesary */
		if (maxCount <= wordCount) {
			maxCount *= 2;
			wordArray = realloc(wordArray, maxCount*sizeof(WFile*));
			fprintf( stdout, "THREAD %lu: Re-allocated array of %d character pointers.\n", pthread, maxCount);
		}

	
		/* Delimit individual word */	
		char temp = fgetc(filePointer);
		if (isalnum(temp)) {
			wordLength += 1;
		}

		/* Allocate space for individual word */
		else {
			if (wordLength > 0) {
				/* Rewind filePointer by wordlength number of bytes */
				fseek(filePointer, ((wordLength+1) * -1), SEEK_CUR);
				wordArray[wordCount] = (WFile*)calloc(1,sizeof(WFile));
				
				// Filename Manipulation
				wordArray[wordCount]->file = (char*)calloc((fileNameLen+1), sizeof(char));
				wordArray[wordCount]->file = fileName;
				wordArray[wordCount]->file += '\0';
				
				// Word Manipulation
				wordArray[wordCount]->word = (char*)calloc((wordLength+1), sizeof(char));
				fread(wordArray[wordCount]->word, 1, wordLength, filePointer);
				wordArray[wordCount]->word += '\0';
				fprintf( stdout, "THREAD %lu: Added \"%s\" at index %d.\n", pthread, wordArray[wordCount]->word, wordCount);
				wordCount += 1;
				wordLength = 0;
			}
		}

		
	
	} /* End file iteration loop */

	/* Close file pointer */
	fclose(filePointer);
	fprintf( stdout, "MAIN THREAD: All done (successfully read %d words).\n", wordCount);

	fprintf( stdout, "MAIN THREAD: Words containing substring \"%s\" are:\n", findWord);
	/* Loop through and find occurrences of requested substring */
	int j;
	
	for (j = 0; j < wordCount; ++j) {
		if (strstr(wordArray[j]->word, findWord) != NULL) {
			fprintf( stdout, "MAIN THREAD: %s (from \"%s\")\n", wordArray[j]->word, wordArray[j]->file);
		}
	}

	/* Free space for individual words */
	
	for (i = 0; i < wordCount; ++i) {
		free(wordArray[i]->word);
		//free(wordArray[i]->file);
		free(wordArray[i]);
	}

	/* Free space for array of POINTERS */
	free(wordArray);

	/* Yaaaaaaaaya! */
	return EXIT_SUCCESS;
}

/* Number of words that caused out-of-memory case: 156231393 */

#if 0

	\    / _  _  _  _  _ |_    __|_ _. _  _  _|
	 \/\/ (_)(_)(_)(_)(_)| |  _\ | | || |(_|_\.
		                              _|   

#endif

