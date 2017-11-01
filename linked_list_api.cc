#include "linked_list_api.h"

using namespace std;

Node* newnode(Key key, Value value) {
	Node *node = (Node *)malloc(sizeof(Node));
	if (kType == INT) {
		node->key.intKey = key.intKey;
	} else {
		node->key.strKey = key.strKey;
	}
	node->value = value;
	node->prev = node->next = NULL;
}

void insert(Node **head, Key key, Value value) {
	Node *node = newnode(key, value);

	if (*head != NULL) {
		node->next = (*head)->next;
		(*head)->next = node;
	}
	*head = node;
}

Node* findUtil(Node *head, Key key, Node **prevNode) {
	if (head == NULL) return head;

	Node *prev = NULL, *temp = head;
	while (temp != NULL) {
		if (kType == INT) {
			if (temp->key.intKey == key.intKey) {
				*prevNode = prev;
				return temp;
			}
		} else if (kType == STRING) {
			if (temp->key.strKey.compare(key.strKey) == 0) {
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

bool findAndRemove(Node **head, Key key) {
	Node *prev = NULL;
	Node *temp = findUtil(*head, key, &prev);

	if (!temp) return false;

	if (temp == *head) {
		*head = (*head)->next;
	} else {
		prev->next = temp->next;
	}

	free(temp);
	return true;
}	
