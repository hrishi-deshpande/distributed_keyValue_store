#ifndef LINKED_LIST_API_H
#define LINKED_LIST_API_H
#include <stdbool.h>
typedef char* Value;

typedef union {
	int intKey;
	char* strKey;
}Key;

typedef enum {
	INT = 1,
	STRING = 2	
}KeyType;

struct node {
	Key key;
	Value value;
	struct node *next;
};

typedef struct node Node;

extern KeyType kType;

void insert(Node *head, Key key, Value value);
_Bool find(Node *head, Key key);
_Bool findAndRemove(Node *head, Key key);
Node* findAndReturn(Node *head, Key key);
void printList(Node *head);
#endif
