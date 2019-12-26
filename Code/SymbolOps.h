#pragma once
#include "Symbol.h"
#include "StackFrame.h"
#include <assert.h>
void SymbolOps_save(Symbol dstVariable, Symbol srcRegister);

void SymbolOps_checkAndModifySelfReference(Symbol srcRegister) {
	List saveList = List_constructor();

	ListIterator it = ListIterator_constructor(Symbol_getReferenceList(srcRegister));
	while (ListIterator_hasNext(it)) {
		Symbol var = (Symbol)ListIterator_next(it);
		if (Symbol_getType(var) == SymbolType_Variable && (List_contains(Symbol_getReferenceList(var), var) == 0)) {
			List_add(saveList, var);
		}
	}
	ListIterator_destroy(it);

	it = ListIterator_constructor(saveList);
	while (ListIterator_hasNext(it)) {
		Symbol var = (Symbol)ListIterator_next(it);
		Variable variable = Symbol_getVariable(var);
		Symbol baseReg = StackFrame_getBaseRegister((StackFrame)Variable_getStackFrame(variable));
		printf("sw %s, %d(%s)\n", Symbol_getName(srcRegister), Variable_getOffset(variable), Symbol_getName(baseReg));
		SymbolOps_save(var, srcRegister);
	}
	ListIterator_destroy(it);

	List_destroy(saveList);
}

void SymbolOps_load(Symbol dstRegister, Symbol srcVariable) {
	assert(Symbol_getType(srcVariable) == SymbolType_Variable && Symbol_getType(dstRegister) == SymbolType_Register);

	SymbolOps_checkAndModifySelfReference(dstRegister);

	Symbol_clearLinks(dstRegister);
	Symbol_link(dstRegister, srcVariable);
}

void SymbolOps_save(Symbol dstVariable, Symbol srcRegister) {
	assert(Symbol_getType(srcRegister) == SymbolType_Register && Symbol_getType(dstVariable) == SymbolType_Variable);
	Symbol_clearLinks(dstVariable);
	Symbol_link(srcRegister, dstVariable);
	Symbol_link(dstVariable, dstVariable);
}

void SymbolOps_assign(Symbol dstRegister, Symbol dstVariable, Symbol paramRegister1, Symbol paramRegister2) {
	assert(Symbol_getType(dstRegister) == SymbolType_Register);
	assert(Symbol_getType(dstVariable) == SymbolType_Variable);
	assert(paramRegister1 == NULL || Symbol_getType(paramRegister1) == SymbolType_Register);
	assert(paramRegister2 == NULL || Symbol_getType(paramRegister2) == SymbolType_Register);
	Symbol_clearLinks(dstRegister);
	Symbol_clearLinks(dstVariable);
	Symbol_link(dstRegister, dstVariable);
}