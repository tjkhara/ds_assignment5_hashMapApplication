#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

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
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement

    // File already open file is passed in as argument

    //***** Code from main BEGIN ******//
    // Populate the concordance with words
    char * theNextWord = nextWord(file); // Get the next word
    while(theNextWord) // while there are words remaining in the file
    {
        // The levenshtein distance will be stored as the value for each word
        // Initially just put 0 for all words
        hashMapPut(map,theNextWord,0);

        // Move to the next word
        theNextWord = nextWord(file);
    }


    // Be sure to free the word after you are done with it here.
    free(theNextWord);

    // Close the file
    fclose(file);

    //***** Code from main END ******//
}


// From
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
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
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");


    // Check if file opened correctly
    if(file == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }

    clock_t timer = clock();
    loadDictionary(file, map);

    // Test print
//    hashMapPrint(map);

    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    
    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
        
        // Implement the spell checker code here..

        // Compare the input buffer to the words in the dictionary
        // Compute their Levenshtein distance
        // Store that distance as the value for each key in the table

        // Traverse the whole map START


        // Pointer to the links
        struct HashLink* linkptr;

        if(!hashMapGet(map,inputBuffer)) // Key is not in the map
        {
            // Generate an array of 5 words that are closest matches to the input buffer
            // based on the lowest Levenshtein distance
            // START

            // Calculate the Levenshtein distance
            // string s1 is input buffer and string s2 is each word in the map (one at a time)
            // Iterate over the entire map

//            printf("\nCapacity of map %d\n", map->capacity);

            int i;

            for(i = 0; i < map->capacity; i++)
            {
                linkptr = map->table[i];

                while(linkptr != NULL)
                {

                    // Calc the LD

                    int ld = levenshtein(inputBuffer, linkptr->key);

                    // Store the LD in the links
                    hashMapPut(map,linkptr->key, ld);

                    linkptr = linkptr->next;
                }
            }

            // END

            // Print the array including the messages
            // "The inputted word .... is spelled incorrectly. Did you mean .... ?

            printf("\nKey is not in the map\n");

//            hashMapPrint(map);

            // Find the 5 closest values

            int min = 1000;
            char* minStr;
            int arr[5];
            char* strs[5];
            int skip = 0;

            for(i = 0; i < 5; i++)
            {
                // Traverse the entire hash map
                for(int j = 0; j < map->capacity; j++)
                {
                    linkptr = map->table[j];

                    while(linkptr != NULL)
                    {
                        skip = 0;

                        if(i == 0)
                        {
                            for(int y = 0; y < i; y++)
                            {
                                if(!strcmp(strs[y],linkptr->key))
                                {
                                    skip = 1;

                                }
                            }

                            if(skip)
                            {
                                linkptr = linkptr->next;
                                continue;
                            }

                            if(min > linkptr->value)
                            {
                                min = linkptr->value;
                                minStr = linkptr->key;
                            }
                        }
                        else // if i > 0
                        {
                            for(int y = 0; y < i; y++)
                            {
                                if(!strcmp(strs[y],linkptr->key))
                                {
                                    skip = 1;

                                }
                            }

                            if(skip)
                            {
                                linkptr = linkptr->next;
                                continue;
                            }

                            if(min > linkptr->value && linkptr->value >= arr[i - 1])
                            {
                                min = linkptr->value;
                                minStr = linkptr->key;
                            }
                        }


                        linkptr = linkptr->next;
                    }
                }

                arr[i] = min;
                strs[i] = minStr;

                min = 1000;
            }

            int z;
            /*printf("\n Printing values \n");
            for(z = 0; z < 5; z++)
            {
                printf("\n%d\n", arr[z]);
            }*/

            printf("\nThe inputted word %s is spelled incorrectly\n", inputBuffer);
            for(z = 0; z < 5; z++)
            {
                printf("\nDid you mean %s?\n",strs[z]);
            }

            /*printf("\nPrinting the strings array\n");
            for(z = 0; z < 5; z++)
            {
                printf("\n%s\n",strs[z]);
            }*/

        }
        else // Key is in the map
        {
            printf("\nThe inputted word %s is spelled correctly\n", inputBuffer);
        }

        // Traverse the whole map END

        // If the user types quit, then exit the program
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }
    hashMapDelete(map);
    return 0;
}