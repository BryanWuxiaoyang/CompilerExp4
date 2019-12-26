#pragma once
#include "SymbolType.h"
#include "Register.h"
#include "Variable.h"
#include "List.h"
#include "ListIterator.h"
#include "assert.h"

struct _Symbol_ {
	SymbolType type;
	const char* name;
	void* element; // can be Register or Variable
	List referenceList;
};
typedef struct _Symbol_* Symbol;

Symbol Symbol_constructor(SymbolType type, const char* name, void* element) {
	Symbol symbol = (Symbol)malloc(sizeof(struct _Symbol_));
	if (symbol != NULL) {
		symbol->type = type;
		symbol->name = name;
		symbol->element = element;
		symbol->referenceList = List_constructor();
	}
	return symbol;
}

SymbolType Symbol_getType(Symbol symbol) {
	return symbol->type;
}

const char* Symbol_getName(Symbol symbol) {
	return symbol->name;
}

Register Symbol_getRegister(Symbol symbol) {
	assert(symbol->type == SymbolType_Register);
	return (Register)symbol->element;
}

Variable Symbol_getVariable(Symbol symbol) {
	assert(symbol->type == SymbolType_Variable);
	return (Variable)symbol->element;
}

List Symbol_getReferenceList(Symbol symbol) {
	return symbol->referenceList;
}

int Symbol_findLinkedSymbol(Symbol symbol, Symbol symbol2) {
	return List_contains(symbol->referenceList, symbol2);
}

void Symbol_link(Symbol symbol, Symbol symbol2) {
	if (symbol == symbol2) {
		List_add(symbol->referenceList, symbol2);
	}
	else {
		List_add(symbol->referenceList, symbol2);
		List_add(symbol2->referenceList, symbol);
	}
}

void Symbol_unlink(Symbol symbol, Symbol symbol2) {
	if (symbol == symbol2) {
		List_remove(symbol->referenceList, symbol2);
	}
	else {
		List_remove(symbol->referenceList, symbol2);
		List_remove(symbol2->referenceList, symbol);
	}
}

void Symbol_clearLinks(Symbol symbol) {
	ListIterator it = ListIterator_constructor(symbol->referenceList);
	while (ListIterator_hasNext(it)) {
		Symbol symbol2 = (Symbol)ListIterator_next(it);
		if (symbol != symbol2) List_remove(symbol2->referenceList, symbol);
		ListIterator_remove(it);
	}
	ListIterator_destroy(it);
}