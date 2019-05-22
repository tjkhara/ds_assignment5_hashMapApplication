/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Tajeshwar Singh Khara
 * Date: May 20th 2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement
    // Loop through the array (table)
    for(int i = 0; i < map->size; i++)
    {
        // Delete the links
        hashLinkDelete(map->table[i]);
    }

}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    // Find index
    int index = HASH_FUNCTION(key) % map->capacity;
    if(index < 0)
    {
        index += map->capacity;
    }

    // At the index go through the link list to find the link with the key

    // Create a pointer to a list node that will traverse the list
    struct HashLink* linkptr;

    linkptr = map->table[index];

    while(linkptr != NULL)
    {
        // strcmp returns 0 if the strings are equal and 1 if not equal
        if(!(int)strcmp(map->table[index]->key,key)) // check
        {
            return &(map->table[index]->value);
        }
        else
        {
            linkptr = linkptr->next;
        }
    }

    // Return pointer to the value of the link
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement

    // save old state
    struct HashLink** oldTable = map->table;
    int oldTableSize = map->size;

    // Reinitialize hash table with double capacity
    hashMapInit(map, capacity);

    // Rehash old elements and store them into the new table
    for(int i = 0; i < oldTableSize; i++)
    {
        struct HashLink* oldLink = oldTable[i];

        // Traverse the linked list in oldTable[i]
        while(oldLink != NULL)
        {
            // Save the next link
            struct HashLink* next = oldLink->next;

            // Rehash and add value to the new table
            hashMapPut(map, oldLink->key, oldLink->value);

            // Free old link
            free(oldLink);

            // Set oldLink to next
            oldLink = next;
        }
    }

    free(oldTable);

}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement

    // Find the correct bucket using the hash function
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0)
        hashIndex = hashIndex + map->capacity;

    // If the map contains the key, update the key
    if(hashMapContainsKey(map,key))
    {
        // Go to the right bucket
        struct HashLink* linkptr = map->table[hashIndex];

        // Search all the links in that bucket
        while(linkptr != NULL)
        {
            // Check to see if the key matches
            if(!strcmp(linkptr->key,key)) //check
            {
                strcpy(linkptr->key,key); // check use strcpy instead
                linkptr->value = value;
                return;
            }
            else
            {
                linkptr = linkptr->next;
            }
        }
    }
    else // if the map does not contain the key
    {
        struct HashLink* newlink;

        // If this is the first link set the next link of the first link (which will be the last link
        // when other links are added to NULL)

        // Since this is not the last link in the linked list make the next pointer
        // equal to map->table[hashIndex]
        // This link will become the new first link
        newlink = hashLinkNew(key, value, map->table[hashIndex]);

        //  Connect the table to this new link
        // This link is the new first link
        map->table[hashIndex] = newlink;
        map->size++;
        if((map->size/(double)map->capacity >= MAX_TABLE_LOAD))
            resizeTable(map, 2*map->capacity);
    }

}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement

    // Find the correct bucket using the hash function
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0)
        hashIndex = hashIndex + map->capacity;

    // For linked list traversal create a pointer
    // Point it at the first element of the linked list
    struct HashLink* linkptr = map->table[hashIndex];

    // Have to make sure that the linked list stays connected
    // Make a pointer that follows the linkptr
    struct HashLink* prevptr = linkptr;

    // For the first link
    // If the key matches delete this link
    if(!strcmp(linkptr->key, key))
    {

        // If the  key is found at the first step
        // Connect the map->table[hashIndex] pointer to the next link
        map->table[hashIndex] = map->table[hashIndex]->next;

        // Delete the first link
        hashLinkDelete(linkptr);
    }
    else // If the key does not match move to the next link
    {
        // Move the linkptr to the next link
        linkptr = linkptr->next;
    }


    // Second link onwards
    // We are deleting all instances
    while(linkptr != NULL)
    {
        if(!strcmp(linkptr->key, key)) // if key matches
        {
            // Key matches
            // Connect previous link to next link
            prevptr->next = linkptr->next;

            // Delete the link that linkptr points to
            hashLinkDelete(linkptr);
        }
        else
        {
            prevptr = linkptr;
            linkptr = linkptr->next;
        }
    }

}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement

    // Find the correct bucket using the hash function
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    if(hashIndex < 0)
        hashIndex = hashIndex + map->capacity;

    // Check to see if the element is there

    // Go to the right bucket
    struct HashLink* linkptr = map->table[hashIndex];

    // Search all the links in that bucket
    while(linkptr != NULL) //check
    {
        // Check to see if the key matches
        if(!strcmp(linkptr->key,key)) //check
        {
            return 1;
        }
        else
        {
            linkptr = linkptr->next;
        }
    }

    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    int number = 0;
    for(int i = 0; i < map->capacity; i++)
    {
        if(map->table[i] == NULL)
        {
            number++;
        }
    }

    return number;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    return map->size/(double)map->capacity;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement

  // Traverse the table and all the lists

  // Make a listptr
  struct HashLink* linkptr;

  printf("\n\n");

  for(int i = 0; i < map->capacity; i++)
  {
      linkptr = map->table[i];

      // Traverse the list at each step in the table
      while(linkptr != NULL)
      {
          printf("key: %s value: %d ", linkptr->key,linkptr->value);
          linkptr = linkptr->next;
      }

      printf("\n");
  }

   
}
