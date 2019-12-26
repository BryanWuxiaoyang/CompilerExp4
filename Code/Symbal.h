#pragma once
#include "SymbalType.h"
#include "Register.h"
#include "Variable.h"
#include "List.h"
#include "ListIterator.h"
#include "assert.h"

struct _Symbal_ {
	SymbalType type;
	const char* name;
	void* element; // can be Register or Variable
	List referenceList;
};
typedef struct _Symbal_* Symbal;

Symbal Symbal_constructor(SymbalType type, const char* name, void* element) {
	Symbal symbal = (Symbal)malloc(sizeof(struct _Symbal_));
	if (symbal != NULL) {
		symbal->type = type;
		symbal->name = name;
		symbal->element = element;
		symbal->referenceList = List_constructor();
	}
	return symbal;
}

SymbalType Symbal_getType(Symbal symbal) {
	return symbal->type;
}

const char* Symbal_getName(Symbal symbal) {
	return symbal->name;
}

Register Symbal_getRegister(Symbal symbal) {
	assert(symbal->type == SymbalType_Register);
	return (Register)symbal->element;
}

Variable Symbal_getVariable(Symbal symbal) {
	assert(symbal->type == SymbalType_Variable);
	return (Variable)symbal->element;
}

List Symbal_getReferenceList(Symbal symbal) {
	return symbal->referenceList;
}

int Symbal_findLinkedSymbal(Symbal symbal, Symbal symbal2) {
	return List_contains(symbal->referenceList, symbal2);
}

void Symbal_link(Symbal symbal, Symbal symbal2) {
	if (symbal == symbal2) {
		List_add(symbal->referenceList, symbal2);
	}
	else {
		List_add(symbal->referenceList, symbal2);
		List_add(symbal2->referenceList, symbal);
	}
}

void Symbal_unlink(Symbal symbal, Symbal symbal2) {
	if (symbal == symbal2) {
		List_remove(symbal->referenceList, symbal2);
	}
	else {
		List_remove(symbal->referenceList, symbal2);
		List_remove(symbal2->referenceList, symbal);
	}
}

void Symbal_clearLinks(Symbal symbal) {
	ListIterator it = ListIterator_constructor(symbal->referenceList);
	while (ListIterator_hasNext(it)) {
		Symbal symbal2 = (Symbal)ListIterator_next(it);
		if (symbal != symbal2) List_remove(symbal2->referenceList, symbal);
		ListIterator_remove(it);
	}
	ListIterator_destroy(it);
}