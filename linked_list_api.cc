#include "linked_list_api.h"
#include "genericApi.h"
#include <stdlib.h>
#include <string.h>

Node* newnode(Key key, Value value) {
	size_t malloc_size = sizeof(Node) + sizeof(Key);
	if (kType == STRING) {
		malloc_size += sizeof(key);
	}
	malloc_size += sizeof(value);

	Node *node = (Node *)malloc(malloc_size);
	if (node == NULL) return NULL;

	size_t runningSize = sizeof(Node);

	if (kType == INT) {
		node->key.intKey = key.intKey;
	} else {
		node->key.strKey = (char*)(node + runningSize);
		runningSize += sizeof(key.strKey);
		strcpy(node->key.strKey, key.strKey);
	}
	node->value = (char*)(node + runningSize);
	strcpy(node->value, value);
	node->next = NULL;
}

void insert(Node *head, Key key, Value value) {
	Node *node = newnode(key, value);

	if (node == NULL) die("OOM");

	node->next = head->next;
	head->next = node;
}

Node* findUtil(Node *head, Key key, Node **prevNode) {
	if (head == NULL) return head;

	Node *prev = head, *temp = head->next;
	while (temp != NULL) {
		if (kType == INT) {
			if (temp->key.intKey == key.intKey) {
				*prevNode = prev;
				return temp;
			}
		} else if (kType == STRING) {
			if (strcmp(temp->key.strKey, key.strKey) == 0) {
				*prevNode = prev;
				return temp;
			}
		}
		prev = temp;
		temp = temp->next;
	}
	return NULL;
}

bool find(Node *head, Key key) {
	Node *prev = NULL;

	Node *temp = findUtil(head, key, &prev);

	return temp?true:false;
}

bool findAndRemove(Node *head, Key key) {
	Node *prev = NULL;
	Node *temp = findUtil(head, key, &prev);

	if (!temp) return false;

	prev->next = temp->next;

	free(temp);
	return true;
}
