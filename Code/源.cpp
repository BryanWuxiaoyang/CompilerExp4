#include "AbstractSyntaxTree.h"
#include "SemanticAnalysis.h"
#include "RegisterSet.h"
#include "StackFrame.h"
#include "SymbalOps.h"
#include <assert.h>


void add(const char* x, const char* y, const char* z, RegisterSet regSet, StackFrame frame) {
	Symbal xVariable = StackFrame_get(frame, x);
	Symbal yVariable = StackFrame_get(frame, y);
	Symbal zVariable = StackFrame_get(frame, z);
	Symbal xReg = RegisterSet_getByName(regSet, "s0");
	Symbal yReg = RegisterSet_getByName(regSet, "s1");
	Symbal zReg = RegisterSet_getByName(regSet, "s2");

	int xOffset = Variable_getOffset(Symbal_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbal_getVariable(yVariable));
	int zOffset = Variable_getOffset(Symbal_getVariable(zVariable));
	Symbal baseReg = StackFrame_getBaseRegister(frame);

	printf("lw $%s, %d($%s)\n", Symbal_getName(yReg), yOffset, Symbal_getName(baseReg));
	printf("lw $%s, %d($%s)\n", Symbal_getName(zReg), zOffset, Symbal_getName(baseReg));

	printf("add $%s, $%s, $%s\n", Symbal_getName(xReg), Symbal_getName(yReg), Symbal_getName(zReg));

	printf("sw $%s, %d($%s)\n", Symbal_getName(xReg), xOffset, Symbal_getName(baseReg));
}

void add2(const char* x, const char* y, const char* z, RegisterSet regSet, StackFrame frame) {
	Symbal xVariable = StackFrame_get(frame, x);
	Symbal yVariable = StackFrame_get(frame, y);
	Symbal zVariable = StackFrame_get(frame, z);

	List forbidList = List_constructor();
	Symbal xReg = RegisterSet_alloc(regSet, xVariable);

	List_add(forbidList, xReg);
	Symbal yReg = RegisterSet_allocWithForbid(regSet, yVariable, forbidList);

	List_add(forbidList, yReg);
	Symbal zReg = RegisterSet_allocWithForbid(regSet, zVariable, forbidList);

	Symbal baseReg = StackFrame_getBaseRegister(frame);
	if (Symbal_findLinkedSymbal(yReg, yVariable) == 0) {
		SymbalOps_load(yReg, yVariable);
		printf("lw %s, %d(%s)\n", Symbal_getName(yReg), Variable_getOffset(Symbal_getVariable(yVariable)), Symbal_getName(baseReg));
	}
	if (Symbal_findLinkedSymbal(zReg, zVariable) == 0) {
		SymbalOps_load(zReg, zVariable);
		printf("lw %s, %d(%s)\n", Symbal_getName(zReg), Variable_getOffset(Symbal_getVariable(zVariable)), Symbal_getName(baseReg));
	}

	int xOffset = Variable_getOffset(Symbal_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbal_getVariable(yVariable));
	int zOffset = Variable_getOffset(Symbal_getVariable(zVariable));

	SymbalOps_assign(xReg, xVariable, yReg, zReg);
	printf("add %s, %s, %s\n", Symbal_getName(xReg), Symbal_getName(yReg), Symbal_getName(zReg));

	SymbalOps_save(xVariable, xReg);
	printf("sw %s, %d(%s)\n", Symbal_getName(xReg), xOffset, Symbal_getName(baseReg));
}

void assign(const char* x, const char* y, RegisterSet regSet, StackFrame frame) {
	Symbal xVariable = StackFrame_get(frame, x);
	Symbal yVariable = StackFrame_get(frame, y);
	Symbal xReg = RegisterSet_getByName(regSet, "s0");
	Symbal yReg = RegisterSet_getByName(regSet, "s1");
	//Symbal xReg = RegisterSet_alloc(regSet, xVariable);
	//Symbal yReg = RegisterSet_alloc(regSet, yVariable);

	int xOffset = Variable_getOffset(Symbal_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbal_getVariable(yVariable));
	Symbal baseReg = StackFrame_getBaseRegister(frame);

	printf("la $%s, %d($%s)\n", Symbal_getName(yReg), yOffset, Symbal_getName(baseReg));
	printf("mov $%s, $%s\n", Symbal_getName(xReg), Symbal_getName(baseReg));
	printf("sw $%s, %d($%s)\n", Symbal_getName(xReg), xOffset, Symbal_getName(baseReg));
}

int main() {
	/*FILE* file = fopen("temp.txt", "r");
	Node node = getSyntaxTreeFromFile(file);
	semAnalysis(node);
	fclose(file);
	printTables();
	printInterCodeList(NULL, NULL);*/

	RegisterSet regSet = RegisterSet_constructor();
	Symbal baseReg = RegisterSet_getByName(regSet, "fp");

	StackFrame frame = StackFrame_constructor(baseReg);

	Symbal x = Symbal_constructor(SymbalType_Variable, "x", Variable_constructor(VariableType_Local, frame, -4));
	Symbal y = Symbal_constructor(SymbalType_Variable, "y", Variable_constructor(VariableType_Local, frame, 0));
	Symbal z = Symbal_constructor(SymbalType_Variable, "z", Variable_constructor(VariableType_Local, frame, 4));
	StackFrame_add(frame, x);
	StackFrame_add(frame, y);
	StackFrame_add(frame, z);


	assign("x", "y", regSet, frame);

	return 0;
}
