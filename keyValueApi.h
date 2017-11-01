#include <string>
#include "linked_list_api.h"

void createStore(KeyType ktype);
void deleteStore(int fd);

string getItem(Key key);
bool putItem(Key key, Value v);
void removeItem(Key key, Value v); 
void scan();
