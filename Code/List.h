#pragma once
#include <stdlib.h>
#include "MyList.h"

struct _List_ {
	MyListHead list;
};
typedef struct _List_* List;

List List_constructor() {
	List list = (List)malloc(sizeof(struct _List_));
	if (list != NULL) {
		list->list = MyList_createList();
	}
	return list;
}

void List_add(List list, void* elem) {
	int found = 0;
	MyListIterator it = MyList_createIterator(list->list);
	while (MyList_hasNext(it)) {
		void* elem2 = MyList_getNext(it);
		if (elem == elem2) {
			found = 1;
			break;
		}
	}
	MyList_destroyIterator(it);
	if (found == 0) {
		MyList_pushElem(list->list, elem);
	}
}

List List_constructor2(List list) {
	List result = List_constructor();
	MyListIterator it = MyList_createIterator(result->list);
	while (MyList_hasNext(it)) {
		void* elem = MyList_getNext(it);
		List_add(result, elem);
	}
	MyList_destroyIterator(it);
	return result;
}

void List_remove(List list, void* elem) {
	MyListIterator it = MyList_createIterator(list->list);
	int found = 0;
	while (MyList_hasNext(it)) {
		void* elem2 = MyList_getNext(it);
		if (elem == elem2) {
			MyList_removePrev(it);
			found = 1;
			break;
		}
	}
	MyList_destroyIterator(it);
}

void List_destroy(List list) {
	MyList_destroyList(list->list);
	free(list);
}

void* List_getFront(List list) {
	return MyList_getFront(list->list);
}

void List_pushFront(List list, void* elem) {
	MyList_pushFront(list->list, elem);
}

void* List_popFront(List list) {
	return MyList_popFront(list->list);
}

void* List_getBack(List list) {
	return MyList_getBack(list->list);
}

void List_pushBack(List list, void* elem) {
	MyList_pushElem(list->list, elem);
}

void* List_popBack(List list) {
	return MyList_pop(list->list);
}

int List_contains(List list, void* elem) {
	int found = 0;
	MyListIterator it = MyList_createIterator(list->list);
	while (MyList_hasNext(it)) {
		void* match = MyList_getNext(it);
		if (elem == match) {
			found = 1;
			break;
		}
	}
	MyList_destroyIterator(it);
	return found;
}

int List_isEmpty(List list) {
	return MyList_isEmpty(list->list);
}