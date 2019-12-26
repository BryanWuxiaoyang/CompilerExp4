#pragma once
#include "List.h"

struct _ListIterator_ {
	MyListIterator iterator;
};
typedef struct _ListIterator_* ListIterator;

ListIterator ListIterator_constructor(List list) {
	ListIterator iterator = (ListIterator)malloc(sizeof(struct _ListIterator_));
	if (iterator != NULL) {
		iterator->iterator = MyList_createIterator(list->list);
	}
	return iterator;
}

int ListIterator_hasNext(ListIterator it) {
	return MyList_hasNext(it->iterator);
}

void* ListIterator_next(ListIterator it) {
	return MyList_getNext(it->iterator);
}

void ListIterator_remove(ListIterator it) {
	MyList_removePrev(it->iterator);
}

void ListIterator_destroy(ListIterator it) {
	MyList_destroyIterator(it->iterator);
	free(it);
}