#include "keyValueApi.h"
#include "keyValueStore.h"
#include <stdlib.h>
#include <string.h>

int hashIndex(Key key) {
	int num = (kType==INT)?key.intKey:sizeof(key.strKey);
	num = num % TABLE_SIZE;
	return num;
}

Value getItem(Key key) {
	int idx = hashIndex(key);
	
	Node *head = hashTable[idx];
	Node* temp = findAndReturn(head, key);
	return temp!=NULL?temp->value:NULL;
}

_Bool containsItem(Key key) {
	int idx = hashIndex(key);
	return find(hashTable[idx], key);
}

void putItem(Key key, Value value) {
	int idx = hashIndex(key);

	Node *head = hashTable[idx];
	insert(head, key, value);	
}

_Bool removeItem(Key key) {
	int idx = hashIndex(key);
	return findAndRemove(hashTable[idx], key);
}

void scan() {
	int i = 0;

	for (i = 0; i < TABLE_SIZE; i++) {
		printList(hashTable[i]);
	}
}

Node* updateItem(Key key, Value newValue) {
	int idx = hashIndex(key);

	Node* temp = findAndReturn(hashTable[idx], key);
	if (temp == NULL) return NULL;

	size_t oldSize = sizeof(temp->value), newSize = sizeof(newValue);
	if (oldSize > newSize) {
		strcpy(temp->value, newValue);
	} else {
		strcpy(temp->value, newValue);		
	}
	return temp;
}
