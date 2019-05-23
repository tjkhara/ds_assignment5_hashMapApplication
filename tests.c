/*
 * CS 261 Data Structures
 * Assignment 5
 */

#include "CuTest.h"
#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// --- Test Helpers ---

typedef struct HistLink HistLink;
typedef struct Histogram Histogram;

struct HistLink
{
    char* key;
    int count;
    HistLink* next;
};

struct Histogram
{
    HistLink* head;
    int size;
};

void histInit(Histogram* hist)
{
    hist->head = NULL;
    hist->size = 0;
}

void histCleanUp(Histogram* hist)
{
    HistLink* link = hist->head;
    while (link != NULL)
    {
        HistLink* next = link->next;
        free(link);
        link = next;
    }
}

void histAdd(Histogram* hist, char* key)
{
    HistLink* link = hist->head;
    while (link != NULL)
    {
        if (strcmp(key, link->key) == 0 )
        {
            link->count++;
            return;
        }
        link = link->next;
    }
    link = malloc(sizeof(HistLink));
    link->key = key;
    link->count = 1;
    link->next = hist->head;
    hist->head = link;
    hist->size++;
}

/**
 * Counts the number of times each key appears in the table.
 * @param hist
 * @param map
 */
void histFromTable(Histogram* hist, HashMap* map)
{
    histInit(hist);
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink* link = map->table[i];
        while (link != NULL)
        {
            histAdd(hist, link->key);
            link = link->next;
        }
    }
}

/**
 * Asserts that each key is unique (count is 1 for each key).
 * @param test
 * @param hist
 */
void assertHistCounts(CuTest* test, Histogram* hist)
{
    HistLink* link = hist->head;
    while (link != NULL)
    {
        CuAssertIntEquals(test, 1, link->count);
        link = link->next;
    }
}

// --- Hash Map tests ---
/*
 * Test cases:
 * - At most one link in each bucket under threshold.
 * - At most one link in each bucket over threshold.
 * - Multiple links in some buckets under threshold.
 * - Multiple links in some buckets over threshold.
 * - Multiple links in some buckets over threshold with duplicates.
 */

/**
 * Tests all hash map functions after adding and removing all of the given keys
 * and values.
 * @param test
 * @param links The key-value pairs to be added and removed.
 * @param notKeys Some keys not in the table to test contains and get.
 * @param numLinks The number of key-value pairs to be added and removed.
 * @param numNotKeys The number of keys not in the table.
 * @param numBuckets The initial number of buckets (capacity) in the table.
 */
void testCase(CuTest* test, HashLink* links, const char** notKeys, int numLinks,
              int numNotKeys, int numBuckets)
{
    HashMap* map = hashMapNew(numBuckets);
    Histogram hist;
    
    // Add links
    for (int i = 0; i < numLinks; i++)
    {
        printf("\nKey values being put Key: %s Value: %d\n", links[i].key, links[i].value); // Delete
        hashMapPut(map, links[i].key, links[i].value);
    }
    
    // Print table
    printf("\nAfter adding all key-value pairs:");
    hashMapPrint(map);
    
    // Check size
    CuAssertIntEquals(test, numLinks, hashMapSize(map));
    
    // Check capacity
    CuAssertIntEquals(test, map->capacity, hashMapCapacity(map));
    
    // Check empty buckets
    int sum = 0;
    for (int i = 0; i < map->capacity; i++)
    {
        if (map->table[i] == NULL)
        {
            sum++;
        }
    }
    CuAssertIntEquals(test, sum, hashMapEmptyBuckets(map));
    
    // Check table load
    CuAssertIntEquals(test, (float)numLinks / map->capacity, hashMapTableLoad(map));
    
    // Check contains and get on valid keys.
    for (int i = 0; i < numLinks; i++)
    {
        CuAssertIntEquals(test, 1, hashMapContainsKey(map, links[i].key));
        int* value = hashMapGet(map, links[i].key);
        CuAssertPtrNotNull(test, value);
        CuAssertIntEquals(test, links[i].value, *value);
    }
    
    // Check contains and get on invalid keys.
    for (int i = 0; i < numNotKeys; i++)
    {
        CuAssertIntEquals(test, 0, hashMapContainsKey(map, notKeys[i]));
        CuAssertPtrEquals(test, NULL, hashMapGet(map, notKeys[i]));
    }
    
    // Check that all links are present and have a unique key.
    histFromTable(&hist, map);
    CuAssertIntEquals(test, numLinks, hist.size);
    assertHistCounts(test, &hist);
    histCleanUp(&hist);
    
    // Remove keys
    for (int i = 0; i < numLinks; i++)
    {
        hashMapRemove(map, links[i].key);
    }
    
    // Print table
    printf("\nAfter removing all key-value pairs:");
    hashMapPrint(map);
    
    // Check size
    CuAssertIntEquals(test, 0, hashMapSize(map));
    
    // Check capacity
    CuAssertIntEquals(test, map->capacity, hashMapCapacity(map));
    
    // Check empty buckets
    CuAssertIntEquals(test, map->capacity, hashMapEmptyBuckets(map));
    
    // Check table load
    CuAssertIntEquals(test, 0, hashMapTableLoad(map));
    
    // Check contains and get on valid keys.
    for (int i = 0; i < numLinks; i++)
    {
        CuAssertIntEquals(test, 0, hashMapContainsKey(map, links[i].key));
        CuAssertPtrEquals(test, NULL, hashMapGet(map, links[i].key));
    }
    
    // Check contains and get on invalid keys.
    for (int i = 0; i < numNotKeys; i++)
    {
        CuAssertIntEquals(test, 0, hashMapContainsKey(map, notKeys[i]));
        CuAssertPtrEquals(test, NULL, hashMapGet(map, notKeys[i]));
    }
    
    // Check that there are no links in the table.
    histFromTable(&hist, map);
    CuAssertIntEquals(test, 0, hist.size);
    assertHistCounts(test, &hist);
    histCleanUp(&hist);
    
    hashMapDelete(map);
}

/**
 * Tests hash map functions for a table with no more than one link
 * in each bucket and without hitting the table load threshold.
 * @param test
 */
void testSingleUnder(CuTest* test)
{
    printf("\n--- Testing single-link chains under threshold ---\n");
    HashLink links[] = {
        { .key = "a", .value = 0, .next = NULL },
        { .key = "c", .value = 1, .next = NULL },
        { .key = "d", .value = 2, .next = NULL },
        { .key = "f", .value = 3, .next = NULL },
        { .key = "g", .value = 4, .next = NULL }
    };
    const char* notKeys[] = { "b", "e", "h" };
    testCase(test, links, notKeys, 5, 3, 10);
}

/**
 * Tests hash map functions for a table with no more than one link
 * in each bucket while hitting the table load threshold.
 * @param test
 */
void testSingleOver(CuTest* test)
{
    printf("\n--- Testing single-link chains over threshold ---\n");
    HashLink links[] = {
        { .key = "a", .value = 0, .next = NULL },
        { .key = "c", .value = 1, .next = NULL },
        { .key = "d", .value = 2, .next = NULL },
        { .key = "f", .value = 3, .next = NULL },
        { .key = "g", .value = 4, .next = NULL }
    };
    const char* notKeys[] = { "b", "e", "h" };
    testCase(test, links, notKeys, 5, 3, 1);
}

/**
 * Tests hash map functions for a table with 2+ links in some buckets without
 * hitting the table load threshold.
 * @param test
 */
void testMultipleUnder(CuTest* test)
{
    printf("\n--- Testing multiple-link chains under threshold ---\n");
    HashLink links[] = {
        { .key = "ab", .value = 0, .next = NULL },
        { .key = "c", .value = 1, .next = NULL },
        { .key = "ba", .value = 2, .next = NULL },
        { .key = "f", .value = 3, .next = NULL },
        { .key = "gh", .value = 4, .next = NULL }
    };
    const char* notKeys[] = { "b", "e", "hg" };
    testCase(test, links, notKeys, 5, 3, 10);
}

/**
 * Tests hash map functions for a table with 2+ links in some buckets while
 * hitting the table load threshold.
 * @param test
 */
void testMultipleOver(CuTest* test)
{
    printf("\n--- Testing multiple-link chains over threshold ---\n");
    HashLink links[] = {
        { .key = "ab", .value = 0, .next = NULL },
        { .key = "c", .value = 1, .next = NULL },
        { .key = "ba", .value = 2, .next = NULL },
        { .key = "f", .value = 3, .next = NULL },
        { .key = "gh", .value = 4, .next = NULL }
    };
    const char* notKeys[] = { "b", "e", "hg" };
    testCase(test, links, notKeys, 5, 3, 1);
}

/**
 * Tests that values are updated when inserting with a key already in the table.
 * Also tests that keys remain unique after insertion (no duplicate links).
 * @param test
 */
void testValueUpdate(CuTest* test)
{
    int numLinks = 5;
    printf("\n--- Testing value updates ---\n");
    HashLink links[] = {
        { .key = "ab", .value = 0, .next = NULL },
        { .key = "c", .value = 1, .next = NULL },
        { .key = "ba", .value = 2, .next = NULL },
        { .key = "ab", .value = 3, .next = NULL },
        { .key = "gh", .value = 4, .next = NULL }
    };
    
    HashMap* map = hashMapNew(1);
    
    // Add links
    for (int i = 0; i < numLinks; i++)
    {
        hashMapPut(map, links[i].key, links[i].value);
    }
    
    // Print table
    printf("\nAfter adding all key-value pairs:");
    hashMapPrint(map);
    
    int* value = hashMapGet(map, "ab");
    CuAssertPtrNotNull(test, value);
    CuAssertIntEquals(test, 3, *value);
    
    Histogram hist;
    histFromTable(&hist, map);
    CuAssertIntEquals(test, numLinks - 1, hist.size);
    assertHistCounts(test, &hist);
    histCleanUp(&hist);
    
    hashMapDelete(map);
}

// --- Test Suite ---

void addAllTests(CuSuite* suite)
{
    SUITE_ADD_TEST(suite, testSingleUnder);
    SUITE_ADD_TEST(suite, testSingleOver);
    SUITE_ADD_TEST(suite, testMultipleUnder);
    SUITE_ADD_TEST(suite, testMultipleOver);
    SUITE_ADD_TEST(suite, testValueUpdate);
}

int main()
{

    /*
    // Test case 1 - just single values added

    HashMap* map = hashMapNew(10);
    hashMapPut(map,"a",0);
    hashMapPut(map,"c",1);
    hashMapPut(map,"d",2);
    hashMapPut(map,"f",3);
    hashMapPut(map,"g",4);

    printf("Printing map after adding a,b,c,d,f and g\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map are %d \n", hashMapEmptyBuckets(map));


    printf("\nThe value of key a is %d", *(hashMapGet(map,"a")));
    printf("\nThe value of key c is %d", *(hashMapGet(map,"c")));
    printf("\nThe value of key d is %d", *(hashMapGet(map,"d")));
    printf("\nThe value of key f is %d", *(hashMapGet(map,"f")));
    printf("\nThe value of key g is %d\n", *(hashMapGet(map,"g")));

    hashMapRemove(map,"a");
    printf("\nPrinting map after removing a\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map after removing a are %d \n", hashMapEmptyBuckets(map));

    // Testing hash map table load
    printf("\nThe table load after adding 5 key value pairs is %f \n", hashMapTableLoad(map));

    hashMapRemove(map,"c");
    printf("\nPrinting map after removing c\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map after removing c are %d \n", hashMapEmptyBuckets(map));

    hashMapRemove(map,"d");
    printf("\nPrinting map after removing d\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map after removing d are %d \n", hashMapEmptyBuckets(map));

    hashMapRemove(map,"f");
    printf("\nPrinting map after removing f\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map after removing f are %d \n", hashMapEmptyBuckets(map));

    // Testing hash map table load
    printf("\nThe table load after removing 4 of the r kv pairs is %f \n", hashMapTableLoad(map));


    hashMapRemove(map,"g");
    printf("\nPrinting map after removing g\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\nThe number of empty buckets in the hash map after removing g are %d \n", hashMapEmptyBuckets(map));*/


    /*// Test case 2
    HashMap* map = hashMapNew(10);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets in the hash map at the start are %d \n\n", hashMapEmptyBuckets(map));


    hashMapPut(map,"ab",0);
    hashMapPut(map,"c",1);
    hashMapPut(map,"ba",2);

    // Testing get function
    printf("\nTesting get function\n");
    int* intptr = hashMapGet(map,"ab");
    printf("\nThe value of key ab is %d\n",*intptr);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after adding 3 values are %d \n\n", hashMapEmptyBuckets(map));

    printf("Printing out the value of the first node at table[5]\n");
    printf("Key: %s Value: %d ", map->table[5]->key, map->table[5]->value);

    printf("Printing out the value of the second node at table[5]\n");
    printf("Key: %s Value: %d ", map->table[5]->next->key, map->table[5]->next->value);

    hashMapPut(map,"f",3);
    hashMapPut(map,"gh",4);

    // Testing hash map table load
    printf("\n\nThe table load after adding 5 key value pairs is %f \n\n", hashMapTableLoad(map));

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after adding 5 values are %d \n\n", hashMapEmptyBuckets(map));

    // Testing out the print function
    printf("\n\nTesting out the print function after adding 5 values\n");
    hashMapPrint(map);

    // Testing remove
    printf("\n\nTesting removal of values:\n");
    hashMapRemove(map,"ab");
    printf("\nMap after removal of ab\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after removing 1 value are %d \n\n", hashMapEmptyBuckets(map));

    hashMapRemove(map,"c");
    printf("\nMap after removal of c\n");
    hashMapPrint(map);

    // Testing hash map table load
    printf("\n\nThe table load after removing 1 key value pair is %f \n\n", hashMapTableLoad(map));

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after removing 2 values are %d \n\n", hashMapEmptyBuckets(map));

    hashMapRemove(map,"ba");
    printf("\nMap after removal of ba\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after removing 3 values are %d \n\n", hashMapEmptyBuckets(map));


    hashMapRemove(map,"f");
    printf("\nMap after removal of f\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after removing 4 values are %d \n\n", hashMapEmptyBuckets(map));

    hashMapRemove(map,"gh");
    printf("\nMap after removal of gh\n");
    hashMapPrint(map);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets after removing 5 values are %d \n\n", hashMapEmptyBuckets(map));

    // Testing hash map table load
    printf("\n\nThe table load after removing key value pairs is %f \n\n", hashMapTableLoad(map));*/


    // ********** Test 3 ************ //
    // Testing to see if get is working
    /*HashMap* map = hashMapNew(10);

    // Testing hash map empty buckets
    printf("\n\nThe number of empty buckets in the hash map at the start are %d \n\n", hashMapEmptyBuckets(map));


    hashMapPut(map,"ab",0);
    hashMapPut(map,"c",1);
    hashMapPut(map,"ba",2);
    hashMapPut(map,"f",3);
    hashMapPut(map,"gh",4);

    // Testing get function
    printf("\nTesting get function\n");
    int* intptr = hashMapGet(map,"ab");
    printf("\nThe value of key ab is %d\n",*intptr);

    intptr = hashMapGet(map,"c");
    printf("\nThe value of key c is %d\n",*intptr);

    intptr = hashMapGet(map,"ba");
    printf("\nThe value of key ba is %d\n",*intptr);

    intptr = hashMapGet(map,"f");
    printf("\nThe value of key f is %d\n",*intptr);

    intptr = hashMapGet(map,"gh");
    printf("\nThe value of key gh is %d\n",*intptr);*/

    // ********** Test 3 ************ //

    // ********** Test 4 ************ //

   /* HashMap* map = hashMapNew(1);

    hashMapPut(map,"ab",0);

    int* intptr = hashMapGet(map,"ab");
    printf("\nThe value of key ab is %d\n",*intptr);

    hashMapPut(map,"c",1);

    intptr = hashMapGet(map,"c");
    printf("\nThe value of key c is %d\n",*intptr);

    hashMapPut(map,"ba",2);

    intptr = hashMapGet(map,"ba");
    printf("\nThe value of key ba is %d\n",*intptr);

    hashMapPut(map,"f",3);

    intptr = hashMapGet(map,"f");
    printf("\nThe value of key f is %d\n",*intptr);

    hashMapPut(map,"gh",4);

    intptr = hashMapGet(map,"gh");
    printf("\nThe value of key gh is %d\n",*intptr);*/


    // ********** Test 4 ************ //


    // ********** Test 5 ************ //

    /*HashMap* map = hashMapNew(1);

    hashMapPut(map,"ab",0);
    hashMapPut(map,"c",1);
    hashMapPut(map,"ba",2);
    hashMapPut(map,"ab",3);
    hashMapPut(map,"gh",4);

    hashMapPrint(map);

    int* value = hashMapGet(map,"ab");

    printf("\nThe value of ab is %d\n",*value);*/


    // ********** Test 5 ************ //




    // Original test case start
    CuSuite* suite = CuSuiteNew();
    addAllTests(suite);
    CuSuiteRun(suite);
    CuString* output = CuStringNew();
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("\n%s\n", output->buffer);
    CuStringDelete(output);
    CuSuiteDelete(suite);
    // Original test case end
    return 0;
}

