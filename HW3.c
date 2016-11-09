#if 0

	Peter Ko
	Operating Systems
	HW3 - Multi-threading in C using Pthreads

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

/*#define DEBUG*/


/* Create struct to manage words and their respective filenames. */
typedef struct WFile {
	char* file;
	char* word;
} WFile;

/* Global Variables */
/* Initialize counter for individual word length, number counters, character string */
int wordCount = 0;
int maxCount = 8;
WFile ** arrayPTR;

/* global mutex variable */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;    /*****************/

		                              
void * threadFun(void * threadArgs) {

	WFile *args = (WFile*)threadArgs; 
	char * fileName = args->file;
	unsigned int * x = (unsigned int *)malloc( sizeof( unsigned int ) );
	*x = pthread_self();

	unsigned int pthreadOutput = (unsigned int)pthread_self();
	

	/* Create file pointer to read text */
	FILE *filePointer;

	int fileNameLen = strlen(fileName);
	filePointer = fopen(fileName, "r");
	int wordLength = 0;
	

	/* Start loop iterating through file */
	while (!feof(filePointer)) {
	
		/* Re-allocate space for array of POINTERS (realloc) if necesary */
		/* --- START CRITICAL SECTION --- */
		pthread_mutex_lock( &mutex );
		if (maxCount <= wordCount) {
			maxCount *= 2;
			arrayPTR = realloc(arrayPTR, maxCount*sizeof(WFile*));	
			fprintf( stdout, "THREAD %u: Re-allocated array of %d character pointers.\n", pthreadOutput, maxCount);
		}
		pthread_mutex_unlock( &mutex );
		/* --- END CRITICAL SECTION --- */
		
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
  
				/* --- START CRITICAL SECTION --- */
				pthread_mutex_lock( &mutex );
				arrayPTR[wordCount] = (WFile*)calloc(1,sizeof(WFile));
				
				// Filename Manipulation
				arrayPTR[wordCount]->file = (char*)calloc((fileNameLen+1), sizeof(char));
				arrayPTR[wordCount]->file = fileName;
				arrayPTR[wordCount]->file += '\0';
				
				// Word Manipulation
				arrayPTR[wordCount]->word = (char*)calloc((wordLength+1), sizeof(char));
				fread(arrayPTR[wordCount]->word, 1, wordLength, filePointer);
				arrayPTR[wordCount]->word += '\0';
				fprintf( stdout, "THREAD %u: Added \"%s\" at index %d.\n", pthreadOutput, arrayPTR[wordCount]->word, wordCount);
				wordCount += 1;
				wordLength = 0;
				pthread_mutex_unlock( &mutex );
				/* --- END CRITICAL SECTION --- */
			}
		}
		

		
	
	} /* End file iteration loop */

	/* Close file pointer */
	fclose(filePointer);
	
	#ifdef DEBUG
		fprintf(stderr, "Debug wordCount = %d\n", wordCount);
		fprintf (stderr,"Exiting thread1\n");
	#endif

	/* Terminate thread */
	pthread_exit( x );
	return NULL;

}

int main(int argc, char *argv[]) {
	
	/* Check to see if a single file was used */
	if (argc != 3) { 
		fprintf( stderr, "ERROR, invalid number of arguments.\n");
		return EXIT_FAILURE;
	}

	char* findWord = argv[2];

	/* Create and allocate space for array of POINTERS(calloc)*/
	arrayPTR = (WFile **)calloc(8, sizeof(WFile*));
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
		    }

			#ifdef DEBUG
			    if ( S_ISREG( buf.st_mode ) ) {
					fprintf ( stdout, "In Directory: %s -- regular file\n", entries->d_name);
					++fileCount;
			    }
			    else if ( S_ISDIR( buf.st_mode ) ) {
					fprintf ( stdout, "In Directory: %s -- directory\n", entries->d_name);
			    }		
			    else {
					fprintf ( stdout, "In Directory: %s -- the fuck is this\n", entries->d_name);
				}
			#endif

		}
	}
	else {
		fprintf( stderr, "MAIN THREAD: Could not open directory. Exiting Program.\n");
		return EXIT_FAILURE;
	}

	pthread_t tid[ fileCount ];   /* keep track of the thread IDs */
  	int i, rc;
	
  	/* Allocate thread space */
  	rewinddir(rootdir);

  	WFile** threadArgs = (WFile**)calloc(fileCount, sizeof(WFile*));

	/* create the threads */
	for ( i = 0 ; i < fileCount;  )
	{
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
	 		
	 		threadArgs[i] = calloc(1, sizeof(struct WFile));

	 		/*	
				char* myd_name = calloc(strlen(entries->d_name)+1, sizeof(char));
				strcpy(myd_name, entries->d_name);
			*/

			threadArgs[i]->file = entries->d_name;

			#ifdef DEBUG

				fprintf(stderr, "Starting: %d\n", i);
				fprintf(stderr, "\nthreadArgs.file = %s\n", threadArgs[i]->file);
				fprintf(stderr, "threadArgs address = %p\n", (void*)&threadArgs[i]);
				fprintf(stderr, "threadArgs.word = %s\n\n", threadArgs[i]->word);
			#endif

			tid[i] = i;

			rc = pthread_create( &(tid[i]), NULL, threadFun, (void *)threadArgs[i]);
			
			if ( rc != 0 ) {
			  fprintf( stderr, "MAIN THREAD: Could not create child thread (%d)\n", rc );
			}

			printf( "MAIN THREAD: Assigned \"%s\" to child thread %u.\n", entries->d_name, (unsigned int)tid[i]);

			++i;
	    }
	}

	/* Catch thread termination */
	for (i = 0; i < fileCount; ++i) {
		
		unsigned int * x;
		//fprintf( stderr,"Waiting for thread %lu completion.\n", tid[i]);
		pthread_join( tid[i], (void **)&x );    /* BLOCKING CALL */		
		#ifdef DEBUG
			fprintf(stderr, "Catching: %d\n", i);
			printf( "MAIN THREAD: Joined a child thread that returned %u.\n", *x );
		#endif
		free(threadArgs[i]);
		free( x );

	}
	free (threadArgs);

	fprintf( stdout, "MAIN THREAD: All done (successfully read %d words from %d files).\n", wordCount, fileCount);
	fprintf( stdout, "MAIN THREAD: Words containing substring \"%s\" are:\n", findWord);

	/* Close file directory pointer */
	closedir(rootdir);	

	#ifdef DEBUG
		fprintf(stderr, "Wordcount = %d\n", wordCount);
	#endif
	
	int j;
	for (j = 0; j < wordCount; ++j) {
		if (strstr(arrayPTR[j]->word, findWord) != NULL) {
			fprintf( stdout, "MAIN THREAD: %s (from \"%s\")\n", arrayPTR[j]->word, arrayPTR[j]->file);
		}
	}

	/* Free space for individual words */
	for (i = 0; i < wordCount; ++i) {
		free(arrayPTR[i]->word);
		//free(arrayPTR[i]->file);
		free(arrayPTR[i]);
	}
	

	/* Free space for array of POINTERS */
	free(arrayPTR);



	/* Yaaaaaaaaya! */
	return EXIT_SUCCESS;
}

/* Number of words that caused out-of-memory case: 156231393 */

#if 0

	\    / _  _  _  _  _ |_    __|_ _. _  _  _|
	 \/\/ (_)(_)(_)(_)(_)| |  _\ | | || |(_|_\.
		                              _|   

#endif

