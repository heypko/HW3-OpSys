#if 0

	Peter Ko
	Operating Systems
	HW1 - C Strings and Dynamic Allocation of Memory

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <dirent.h>


// Create struct to manage words and their respective filenames.
typedef struct WFile WFile;
struct WFile {
	char* file;
	char* word;
};

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

	fprintf( stdout, "Allocated initial array of 8 character pointers.\n");






	// Create & Assign threads to multiple files in single directory





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
			fprintf( stdout, "Re-allocated array of %d character pointers.\n", maxCount);
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
				fprintf( stdout, "Added \"%s\" at index %d.\n", wordArray[wordCount]->word, wordCount);
				wordCount += 1;
				wordLength = 0;
			}
		}

		
	
	} /* End file iteration loop */

	/* Close file pointer */
	fclose(filePointer);
	fprintf( stdout, "All done (successfully read %d words).\n", wordCount);

	fprintf(stdout, "Words containing substring \"%s\" are:\n", findWord);
	/* Loop through and find occurrences of requested substring */
	int j;
	
	for (j = 0; j < wordCount; ++j) {
		if (strstr(wordArray[j]->word, findWord) != NULL) {
			fprintf( stdout, "%s (from \"%s\")\n", wordArray[j]->word, wordArray[j]->file);
		}
	}

	/* Free space for individual words */
	int i;
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
