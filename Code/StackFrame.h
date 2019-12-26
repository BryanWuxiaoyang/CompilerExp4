#pragma once
#include "Symbol.h"
#include <string.h>
#include <stdlib.h>

struct _StackFrame_ {
	Symbol baseReg;

	List symbolList;
};
typedef struct _StackFrame_* StackFrame;

StackFrame StackFrame_constructor(Symbol baseReg) {
	assert(baseReg->type == SymbolType_Register);
	StackFrame frame = (StackFrame)malloc(sizeof(struct _StackFrame_));
	if (frame != NULL) {
		frame->baseReg = baseReg;
		frame->symbolList = List_constructor();
	}
	return frame;
}

void StackFrame_add(StackFrame frame, Symbol variable) {
	assert(variable->type == SymbolType_Variable);
	List_add(frame->symbolList, variable);
}

void StackFrame_remove(StackFrame frame, Symbol variable) {
	assert(variable->type == SymbolType_Variable);
	List_remove(frame->symbolList, variable);
}

Symbol StackFrame_get(StackFrame frame, const char* name) {
	Symbol result = NULL;
	ListIterator it = ListIterator_constructor(frame->symbolList);
	while (ListIterator_hasNext(it)) {
		Symbol symbol = (Symbol)ListIterator_next(it);
		if (strcmp(symbol->name, name) == 0) {
			result = symbol;
			break;
		}
	}
	ListIterator_destroy(it);
	return result;
}

Symbol StackFrame_getBaseRegister(StackFrame frame) {
	return frame->baseReg;
}