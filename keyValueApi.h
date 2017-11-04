#ifndef KEYVALUEAPI_H
#define KEYVALUEAPI_H
#include <stdbool.h>
#include "linked_list_api.h"

void createStore(KeyType ktype);
void deleteStore(int fd);

Value getItem(Key key);
_Bool containsItem(Key key);
void putItem(Key key, Value v);
_Bool removeItem(Key key); 
void scan();
Node* updateItem(Key key, Value newValue);
#endif
