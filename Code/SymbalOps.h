#pragma once
#include "Symbal.h"
#include "StackFrame.h"
#include <assert.h>
void SymbalOps_save(Symbal dstVariable, Symbal srcRegister);

void SymbalOps_checkAndModifySelfReference(Symbal srcRegister) {
	List saveList = List_constructor();

	ListIterator it = ListIterator_constructor(Symbal_getReferenceList(srcRegister));
	while (ListIterator_hasNext(it)) {
		Symbal var = (Symbal)ListIterator_next(it);
		if (Symbal_getType(var) == SymbalType_Variable && (List_contains(Symbal_getReferenceList(var), var) == 0)) {
			List_add(saveList, var);
		}
	}
	ListIterator_destroy(it);

	it = ListIterator_constructor(saveList);
	while (ListIterator_hasNext(it)) {
		Symbal var = (Symbal)ListIterator_next(it);
		Variable variable = Symbal_getVariable(var);
		Symbal baseReg = StackFrame_getBaseRegister((StackFrame)Variable_getStackFrame(variable));
		printf("sw %s, %d(%s)\n", Symbal_getName(srcRegister), Variable_getOffset(variable), Symbal_getName(baseReg));
		SymbalOps_save(var, srcRegister);
	}
	ListIterator_destroy(it);

	List_destroy(saveList);
}

void SymbalOps_load(Symbal dstRegister, Symbal srcVariable) {
	assert(Symbal_getType(srcVariable) == SymbalType_Variable && Symbal_getType(dstRegister) == SymbalType_Register);

	SymbalOps_checkAndModifySelfReference(dstRegister);

	Symbal_clearLinks(dstRegister);
	Symbal_link(dstRegister, srcVariable);
}

void SymbalOps_save(Symbal dstVariable, Symbal srcRegister) {
	assert(Symbal_getType(srcRegister) == SymbalType_Register && Symbal_getType(dstVariable) == SymbalType_Variable);
	Symbal_clearLinks(dstVariable);
	Symbal_link(srcRegister, dstVariable);
	Symbal_link(dstVariable, dstVariable);
}

void SymbalOps_assign(Symbal dstRegister, Symbal dstVariable, Symbal paramRegister1, Symbal paramRegister2) {
	assert(Symbal_getType(dstRegister) == SymbalType_Register);
	assert(Symbal_getType(dstVariable) == SymbalType_Variable);
	assert(paramRegister1 == NULL || Symbal_getType(paramRegister1) == SymbalType_Register);
	assert(paramRegister2 == NULL || Symbal_getType(paramRegister2) == SymbalType_Register);
	Symbal_clearLinks(dstRegister);
	Symbal_clearLinks(dstVariable);
	Symbal_link(dstRegister, dstVariable);
}