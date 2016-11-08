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


/* Global Variables */
/* Initialize counter for individual word length, number counters, character string */
int wordLength = 0;
int wordCount = 0;
int maxCount = 8;
void * arrayPTR;


/* Create struct to manage words and their respective filenames. */
typedef struct WFile WFile;
struct WFile {
	char* file;
	char* word;
};

		                              
void * threadFun(void * threadArgs) {
	
	WFile ** wordArray = arrayPTR;

	WFile *args = (struct WFile *)threadArgs; 

	char * fileName = args->file;
	char * findWord = args->word;

	fprintf(stderr, "File Name = %s \n", fileName);
	fprintf(stderr, "Find Word = %s \n", findWord);
	fprintf(stderr, "It's not doing anything. \n");


	unsigned int * x = (unsigned int *)malloc( sizeof( unsigned int ) );
	pthread_t pthread = pthread_self(); /* thread id */

	*x = pthread_self();
	fprintf (stderr, "Pthread = %u\n", *x);

	/* Create file pointer to read text */
	FILE *filePointer;

	int fileNameLen = strlen(fileName);
	//fprintf(stderr, "%d\n", fileNameLen);
	filePointer = fopen(fileName, "r");

	

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
				fprintf( stdout, "THREAD %u: Added \"%s\" at index %d.\n", *x, wordArray[wordCount]->word, wordCount);
				wordCount += 1;
				wordLength = 0;
			}
		}

		
	
	} /* End file iteration loop */

	/* Close file pointer */
	fclose(filePointer);

	/* Get Thread ID */
	
	//*x = pthread_self();
	fprintf (stderr, "Pthread = %lu\n", pthread);
	fprintf(stderr, "Debug wordCount = %d\n", wordCount);
	fprintf (stderr,"Exiting thread1\n");
	pthread_exit( x );
	return NULL;

}

int main(int argc, char *argv[]) {
	
	/* check to see if a single file was used */
	if (argc != 3) { 
		fprintf( stderr, "ERROR, invalid number of arguments.\n");
		return EXIT_FAILURE;
	}


	char* findWord = argv[2];

	/* Create and allocate space for array of POINTERS(calloc)*/
	WFile **wordArray = (WFile **)calloc(8, sizeof(WFile*));
	arrayPTR = wordArray;
	fprintf( stdout, "MAIN THREAD: Allocated initial array of 8 character pointers.\n");


	/* Open directory */
	DIR *rootdir;
	struct dirent *entries;
	int fileCount = 0;
	chdir(argv[1]);
	rootdir = opendir(".");

	if (rootdir != NULL)
	{
		while ((entries = readdir(rootdir)) != NULL) {
			
			struct stat buf;

		    int lstatCheck = lstat( entries->d_name, &buf );

	        if ( lstatCheck == -1 ) {
			  perror( "lstat() failed" );
			  return EXIT_FAILURE;
			}

		    if ( S_ISREG( buf.st_mode ) ) {
		      ++fileCount;
			  fprintf ( stdout, "In Directory: %s -- regular file\n", entries->d_name);
		    }
		    else if ( S_ISDIR( buf.st_mode ) ) {
		      fprintf ( stdout, "In Directory: %s -- directory\n", entries->d_name);
		    }		
		    else {
			  fprintf ( stdout, "In Directory: %s -- the fuck is this\n", entries->d_name);
			}

		}

		fprintf( stdout, "FileCount = %d\n", fileCount);
	}

	else {
		fprintf( stderr, "MAIN THREAD: Could not open directory. Exiting Program.\n");
		return EXIT_FAILURE;
	}

	pthread_t tid[ fileCount ];   /* keep track of the thread IDs */
  	int i, rc;
  	

  	/* Allocate thread space */
  	rewinddir(rootdir);

  	//int emptyCheck = 0;

	/* create the threads */
	for ( i = 0 ; i < fileCount; i++ )
	{
		fprintf(stderr, "Starting: %d\n", i);
		
		if ((entries = readdir(rootdir)) == NULL) {
			break;
		}	

		
		
		struct stat filebuf;

	    int fileCheck = lstat( entries->d_name, &filebuf );

	    if ( fileCheck == -1 ) {
			perror( "lstat() failed" );
			return EXIT_FAILURE;
		}

	    if ( S_ISREG( filebuf.st_mode ) ) {
			printf( "MAIN THREAD: Assigned \"%s\" to child thread %lu.\n", entries->d_name, tid[i]);
			WFile threadArgs;
			threadArgs.file = entries->d_name;
			threadArgs.word = findWord;
			rc = pthread_create( &(tid[i]), NULL, threadFun, (void *)&threadArgs);
			//rc = pthread_create( &tid[i], NULL, threadFun, (void *)&threadArgs);
	      	if ( rc != 0 ) {
			  fprintf( stderr, "MAIN THREAD: Could not create child thread (%d)\n", rc );
			}
	    }
		--i;	
		// Stuff with threads

	}

	/* Catch thread termination */
	for (i = 0; i < fileCount; ++i) {
		fprintf(stderr, "Catching: %d\n", i);
		unsigned int * x;
		//fprintf( stderr,"Waiting for thread %lu completion.\n", tid[i]);
		//sleep(10);
		pthread_join( tid[i], (void **)&x );    /* BLOCKING CALL */		
		printf( "MAIN: Joined a child thread that returned %u.\n", *x );
		free ( x );
	}

	fprintf( stdout, "MAIN THREAD: All done (successfully read %d words).\n", wordCount);
	fprintf( stdout, "MAIN THREAD: Words containing substring \"%s\" are:\n", findWord);
	/* Loop through and find occurrences of requested substring */
	
	int j;
	
	for (j = 0; j < wordCount; ++j) {
		if (strstr(wordArray[j]->word, findWord) != NULL) {
			fprintf( stdout, "MAIN THREAD: %s (from \"%s\")\n", wordArray[j]->word, wordArray[j]->file);
		}
	}

		

	//pthread_t pthread = pthread_self(); /* thread id
	// // Create & Assign threads to multiple files in single directory
	// /* create pipes */ 
	// int pipeCount;
	// int **pipeArray = malloc(fileCount*sizeof(int*));
	// for (pipeCount = 0; pipeCount < fileCount - 1; ++pipeCount) {
	// 	pipeArray[pipeCount] = malloc(2*sizeof(int));
	// }

	fprintf(stderr, "BUTTS\n");


	// /* Free space for individual words */
	// int i;
	// for (i = 0; i < wordCount; ++i) {
	// 	free(wordArray[i]->word);
	// 	//free(wordArray[i]->file);
	// 	free(wordArray[i]);
	// }


	/* Free space for array of POINTERS */
	//free(wordArray);

	/* Yaaaaaaaaya! */
	return EXIT_SUCCESS;
}

/* Number of words that caused out-of-memory case: 156231393 */

#if 0

	\    / _  _  _  _  _ |_    __|_ _. _  _  _|
	 \/\/ (_)(_)(_)(_)(_)| |  _\ | | || |(_|_\.
		                              _|   

#endif

