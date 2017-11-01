#include <string>

typedef std::string Value;

typdef union {
	int Intkey;
	string strKey;
}Key;

typedef enum {
	INT = 1,
	STRING = 2	
}KeyType;


struct node {
	Key key;
	Value value;
	struct node *prev, *next;
};

typedef struct node Node;

extern KeyType kType;

void insert(Node **head, Key key, Value value);
bool find(Node *head, Key key);
bool findAndRemove(Node **head, Key key);
