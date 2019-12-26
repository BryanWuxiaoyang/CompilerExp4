#pragma once
#include <stdlib.h>
#include "VariableType.h"
struct _Variable_ {
	VariableType type;
	void* stackFrame;
	int offset;
};
typedef struct _Variable_* Variable;

Variable Variable_constructor(VariableType type, void* stackFrame, int offset) {
	Variable variable = (Variable)malloc(sizeof(struct _Variable_));
	if (variable != NULL) {
		variable->type = type;
		variable->stackFrame = stackFrame;
		variable->offset = offset;
	}
	return variable;
}

VariableType Variable_getType(Variable var) {
	return var->type;
}

void* Variable_getStackFrame(Variable var) {
	return var->stackFrame;
}

int Variable_getOffset(Variable var) {
	return var->offset;
}

void Variable_setOffset(Variable var, int offset) {
	var->offset = offset;
}
