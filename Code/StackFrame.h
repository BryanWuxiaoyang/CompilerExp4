#pragma once
#include "Symbal.h"
#include <string.h>
#include <stdlib.h>

struct _StackFrame_ {
	Symbal baseReg;

	List symbalList;
};
typedef struct _StackFrame_* StackFrame;

StackFrame StackFrame_constructor(Symbal baseReg) {
	assert(baseReg->type == SymbalType_Register);
	StackFrame frame = (StackFrame)malloc(sizeof(struct _StackFrame_));
	if (frame != NULL) {
		frame->baseReg = baseReg;
		frame->symbalList = List_constructor();
	}
	return frame;
}

void StackFrame_add(StackFrame frame, Symbal variable) {
	assert(variable->type == SymbalType_Variable);
	List_add(frame->symbalList, variable);
}

void StackFrame_remove(StackFrame frame, Symbal variable) {
	assert(variable->type == SymbalType_Variable);
	List_remove(frame->symbalList, variable);
}

Symbal StackFrame_get(StackFrame frame, const char* name) {
	Symbal result = NULL;
	ListIterator it = ListIterator_constructor(frame->symbalList);
	while (ListIterator_hasNext(it)) {
		Symbal symbal = (Symbal)ListIterator_next(it);
		if (strcmp(symbal->name, name) == 0) {
			result = symbal;
			break;
		}
	}
	ListIterator_destroy(it);
	return result;
}

Symbal StackFrame_getBaseRegister(StackFrame frame) {
	return frame->baseReg;
}