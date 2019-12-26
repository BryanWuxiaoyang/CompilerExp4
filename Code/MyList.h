#pragma once
#include <stdlib.h>
#include <stdio.h>

struct _MyListNode_ {
	void* elem;
	struct _MyListNode_* next;
	struct _MyListNode_* prev;
};
typedef struct _MyListNode_* MyListNode;
typedef struct _MyListNode_* MyListHead;

struct _MyListIterator_ {
	MyListHead head;
	MyListNode next;
	MyListNode prev;
};
typedef struct _MyListIterator_* MyListIterator;

MyListIterator MyList_createIterator(MyListHead head) {
	MyListIterator handlerIt = (MyListIterator)malloc(sizeof(struct _MyListIterator_));
	if (handlerIt) {
		handlerIt->head = head;
		handlerIt->next = head->next;
		handlerIt->prev = head;
	}
	return handlerIt;
}

MyListIterator MyList_createReverseIterator(MyListHead head) {
	MyListIterator handlerIt = (MyListIterator)malloc(sizeof(struct _MyListIterator_));
	if(handlerIt) {
		handlerIt->head = head;
		handlerIt->next = head;
		handlerIt->prev = head->prev;
	}
	return handlerIt;
}

MyListIterator MyList_copyIterator(MyListIterator srcIt){
	MyListIterator dstIt = (MyListIterator)malloc(sizeof(struct _MyListIterator_));
	if(dstIt){
		dstIt->head = srcIt->head;
		dstIt->prev = srcIt->prev;
		dstIt->next = srcIt->next;
	}
	return dstIt;
}

void* MyList_getNext(MyListIterator handlerIt) {
	if (handlerIt->next == handlerIt->head) return NULL;
	void* elem = handlerIt->next->elem;
	handlerIt->prev = handlerIt->next;
	handlerIt->next = handlerIt->next->next;
	return elem;
}

void* MyList_peekNext(MyListIterator handlerIt) {
	return handlerIt->next->elem;
}

int MyList_hasNext(MyListIterator handlerIt) {
	return (handlerIt->next == handlerIt->head) ? 0 : 1;
}

void* MyList_getPrev(MyListIterator handlerIt) {
	if (handlerIt->prev == handlerIt->head) return NULL;
	void* elem = handlerIt->prev->elem;
	handlerIt->next = handlerIt->prev;
	handlerIt->prev = handlerIt->prev->prev;
	return elem;
}

void* MyList_peekPrev(MyListIterator handlerIt) {
	return handlerIt->prev->elem;
}

int MyList_hasPrev(MyListIterator handlerIt) {
	return (handlerIt->prev == handlerIt->head) ? 0 : 1;
}


void MyList_destroyIterator(MyListIterator handlerIt) {
	free(handlerIt);
}



MyListHead MyList_createList() {
	MyListHead head = (MyListHead)malloc(sizeof(struct _MyListNode_));
	if (head) {
		head->elem = NULL;
		head->next = head;
		head->prev = head;
	}
	return head;
}

MyListNode MyList_wrapNode(void* elem) {
	MyListNode node = (MyListNode)malloc(sizeof(struct _MyListNode_));
	if (node) {
		node->elem = elem;
		node->next = node;
		node->prev = node;
	}
	return node;
}

void MyList_pushNode(MyListHead head, MyListNode node) {
	node->next = head;
	node->prev = head->prev;
	head->prev->next = node;
	head->prev = node;
}

void MyList_pushElem(MyListHead head, void* elem) {
	MyList_pushNode(head, MyList_wrapNode(elem));
}

int MyList_isEmpty(MyListHead head) {
	return head->next == head ? 1 : 0;
}

void* MyList_pop(MyListHead head) {
	void* elem = NULL;
	if (MyList_isEmpty(head) == 0) {
		MyListNode p = head->prev;
		elem = p->elem;
		MyListNode q = head->prev->prev;
		q->next = head;
		head->prev = q;
		free(p);
	}
	return elem;
}

void* MyList_removePrev(MyListIterator handlerIt) {
	MyListNode node1 = handlerIt->prev->prev;
	MyListNode node2 = handlerIt->prev;
	MyListNode node3 = handlerIt->next;
	node1->next = node3;
	node3->prev = node1;
	handlerIt->prev = node1;
	handlerIt->next = node3;
	return node2->elem;
}

void* MyList_removeNext(MyListIterator handlerIt) {
	MyListNode node1 = handlerIt->prev;
	MyListNode node2 = handlerIt->next;
	MyListNode node3 = handlerIt->next->next;
	node1->next = node3;
	node3->prev = node1;
	handlerIt->prev = node1;
	handlerIt->next = node3;
	return node2->elem;
}

void MyList_clear(MyListHead handlerList) {
	MyListNode p = handlerList->next;
	while (p != handlerList) {
		MyListNode q = p;
		p = p->next;
		free(q);
	}
	handlerList->next = handlerList->prev = handlerList;
}

void MyList_insert(MyListIterator handlerIt, void* elem) {
	MyListNode node1 = handlerIt->prev;
	MyListNode node2 = MyList_wrapNode(elem);
	MyListNode node3 = handlerIt->next;
	node2->prev = node1;
	node2->next = node3;
	node1->next = node2;
	node3->prev = node2;
	handlerIt->prev = node1;
	handlerIt->next = node2;
}

void MyList_destroyList(MyListHead handlerList) {
	MyList_clear(handlerList);
	free(handlerList);
}

void* MyList_getBack(MyListHead handlerList) {
	return handlerList->prev->elem;
}

void* MyList_getFront(MyListHead handlerList) {
	return handlerList->next->elem;
}

void MyList_pushFront(MyListHead list, void* elem) {
	MyListNode node1 = list;
	MyListNode node2 = list->next;
	MyListNode insertNode = MyList_wrapNode(elem);
	insertNode->prev = node1;
	insertNode->next = node2;
	node1->next = insertNode;
	node2->prev = insertNode;
}

void* MyList_popFront(MyListHead list) {
	MyListNode node1 = list;
	MyListNode node2 = list->next->next;
	MyListNode removeNode = list->next;
	node1->next = node2;
	node2->next = node1;
	free(removeNode);
	return removeNode->elem;
}

void MyList_mergeList_intersect(MyListHead dstList, MyListHead srcList) {
	MyListIterator dstIt = MyList_createIterator(dstList);
	while (MyList_hasNext(dstIt)) {
		void* dstElem = MyList_getNext(dstIt);
		int suc = 0;

		MyListIterator srcIt = MyList_createIterator(srcList);
		while (MyList_hasNext(srcIt)) {
			void* srcElem = MyList_getNext(srcIt);
			if (dstElem == srcElem) {
				suc = 1;
				break;
			}
		}
		MyList_destroyIterator(srcIt);

		if (suc == 0) MyList_removePrev(dstIt);
	}

	MyList_destroyIterator(dstIt);
}