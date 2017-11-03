#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H
#include <stdlib.h>

#define TABLE_SIZE 67
#define MAX_VALUE_SIZE 256

#include "linked_list_api.h"
extern void* curAddr;
extern void* maxAddr;
extern Node *hashTable[TABLE_SIZE];
#endif
