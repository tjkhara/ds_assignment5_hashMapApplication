#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
//Fix me: Do the necessary change to make the implementation //case-insensitive  
   while (1) 
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')  
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            // .............
            // ******** make the implementation case-insensitive start ***** //
            // If the character is upper case convert it to lower case
            if(c >= 'A' && c <= 'Z')
            {
                c = c + 32;
            }
            // ******** make the implementation case-insensitive end ***** //
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    const char* fileName = "input1.txt";
    if (argc > 1)
    {
        fileName = argv[1];
    }
    printf("Opening file: %s\n", fileName);

    // Open the file
    FILE* fp;

    fp = fopen(fileName,"r");

    if(fp == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }
    
    clock_t timer = clock();
    
    HashMap* map = hashMapNew(10);
    
    // --- Concordance code begins here ---

    // Populate the concordance with words
    char * theNextWord = nextWord(fp); // Get the next word
    while(theNextWord) // while there are words remaining in the file
    {
        // Check if the word is already in the map
        if(hashMapContainsKey(map,theNextWord))
        {
            // If the key already exists,get its value, increase its value, and put it back in
            // If the key already exists, put should update the value of the key

            // Get the value in the key already
            int value = *hashMapGet(map,theNextWord);
            // increase that value by 1
            value++;
            // Update that value in the hashmap
            hashMapPut(map,theNextWord,value);
        }
        else // If the key is not in the map add it to the map with value 1
        {
            hashMapPut(map,theNextWord,1);
        }

      /*  // Printing map before moving to the next word
        printf("\n-------------------\n");
        printf("Printing map before moving to the next word: \n");
        hashMapPrint(map);
        printf("\n-------------------\n");*/

        // Move to the next word
        theNextWord = nextWord(fp);
    }


    // Be sure to free the word after you are done with it here.
    free(theNextWord);

    // Close the file
    fclose(fp);

    // Print the hash map
//    hashMapPrint(map);

    // Create a pointer for traversal
    struct HashLink* linkptr;

    // Print the hash map
    for(int i = 0; i < map->capacity; i++)
    {
        linkptr = map->table[i];

        if(linkptr != NULL)
        {
            printf("\nBucket %d -> ",i);
        }

        while(linkptr != NULL)
        {

            printf("(%s, %d) ->", linkptr->key, linkptr->value);

            // Move the linkptr to the next link
            linkptr = linkptr->next;
        }

    }

    // --- Concordance code ends here ---
    
    
    
    timer = clock() - timer;
    printf("\nRan in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    printf("Empty buckets: %d\n", hashMapEmptyBuckets(map));
    printf("Number of links: %d\n", hashMapSize(map));
    printf("Number of buckets: %d\n", hashMapCapacity(map));
    printf("Table load: %f\n", hashMapTableLoad(map));
    
    hashMapDelete(map);
    return 0;
}