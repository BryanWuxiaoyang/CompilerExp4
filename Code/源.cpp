#include "AbstractSyntaxTree.h"
#include "SemanticAnalysis.h"
#include "RegisterSet.h"
#include "StackFrame.h"
#include "SymbolOps.h"
#include <assert.h>


void add(const char* x, const char* y, const char* z, RegisterSet regSet, StackFrame frame) {
	Symbol xVariable = StackFrame_get(frame, x);
	Symbol yVariable = StackFrame_get(frame, y);
	Symbol zVariable = StackFrame_get(frame, z);
	Symbol xReg = RegisterSet_getByName(regSet, "s0");
	Symbol yReg = RegisterSet_getByName(regSet, "s1");
	Symbol zReg = RegisterSet_getByName(regSet, "s2");

	int xOffset = Variable_getOffset(Symbol_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbol_getVariable(yVariable));
	int zOffset = Variable_getOffset(Symbol_getVariable(zVariable));
	Symbol baseReg = StackFrame_getBaseRegister(frame);

	printf("lw $%s, %d($%s)\n", Symbol_getName(yReg), yOffset, Symbol_getName(baseReg));
	printf("lw $%s, %d($%s)\n", Symbol_getName(zReg), zOffset, Symbol_getName(baseReg));

	printf("add $%s, $%s, $%s\n", Symbol_getName(xReg), Symbol_getName(yReg), Symbol_getName(zReg));

	printf("sw $%s, %d($%s)\n", Symbol_getName(xReg), xOffset, Symbol_getName(baseReg));
}

void add2(const char* x, const char* y, const char* z, RegisterSet regSet, StackFrame frame) {
	Symbol xVariable = StackFrame_get(frame, x);
	Symbol yVariable = StackFrame_get(frame, y);
	Symbol zVariable = StackFrame_get(frame, z);

	List forbidList = List_constructor();
	Symbol xReg = RegisterSet_alloc(regSet, xVariable);

	List_add(forbidList, xReg);
	Symbol yReg = RegisterSet_allocWithForbid(regSet, yVariable, forbidList);

	List_add(forbidList, yReg);
	Symbol zReg = RegisterSet_allocWithForbid(regSet, zVariable, forbidList);

	Symbol baseReg = StackFrame_getBaseRegister(frame);
	if (Symbol_findLinkedSymbol(yReg, yVariable) == 0) {
		SymbolOps_load(yReg, yVariable);
		printf("lw %s, %d(%s)\n", Symbol_getName(yReg), Variable_getOffset(Symbol_getVariable(yVariable)), Symbol_getName(baseReg));
	}
	if (Symbol_findLinkedSymbol(zReg, zVariable) == 0) {
		SymbolOps_load(zReg, zVariable);
		printf("lw %s, %d(%s)\n", Symbol_getName(zReg), Variable_getOffset(Symbol_getVariable(zVariable)), Symbol_getName(baseReg));
	}

	int xOffset = Variable_getOffset(Symbol_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbol_getVariable(yVariable));
	int zOffset = Variable_getOffset(Symbol_getVariable(zVariable));

	SymbolOps_assign(xReg, xVariable, yReg, zReg);
	printf("add %s, %s, %s\n", Symbol_getName(xReg), Symbol_getName(yReg), Symbol_getName(zReg));

	SymbolOps_save(xVariable, xReg);
	printf("sw %s, %d(%s)\n", Symbol_getName(xReg), xOffset, Symbol_getName(baseReg));
}

void assign(const char* x, const char* y, RegisterSet regSet, StackFrame frame) {
	Symbol xVariable = StackFrame_get(frame, x);
	Symbol yVariable = StackFrame_get(frame, y);
	Symbol xReg = RegisterSet_getByName(regSet, "s0");
	Symbol yReg = RegisterSet_getByName(regSet, "s1");
	//Symbol xReg = RegisterSet_alloc(regSet, xVariable);
	//Symbol yReg = RegisterSet_alloc(regSet, yVariable);

	int xOffset = Variable_getOffset(Symbol_getVariable(xVariable));
	int yOffset = Variable_getOffset(Symbol_getVariable(yVariable));
	Symbol baseReg = StackFrame_getBaseRegister(frame);

	printf("la $%s, %d($%s)\n", Symbol_getName(yReg), yOffset, Symbol_getName(baseReg));
	printf("mov $%s, $%s\n", Symbol_getName(xReg), Symbol_getName(baseReg));
	printf("sw $%s, %d($%s)\n", Symbol_getName(xReg), xOffset, Symbol_getName(baseReg));
}

int main() {
	/*FILE* file = fopen("temp.txt", "r");
	Node node = getSyntaxTreeFromFile(file);
	semAnalysis(node);
	fclose(file);
	printTables();
	printInterCodeList(NULL, NULL);*/

	RegisterSet regSet = RegisterSet_constructor();
	Symbol baseReg = RegisterSet_getByName(regSet, "fp");

	StackFrame frame = StackFrame_constructor(baseReg);

	Symbol x = Symbol_constructor(SymbolType_Variable, "x", Variable_constructor(VariableType_Local, frame, -4));
	Symbol y = Symbol_constructor(SymbolType_Variable, "y", Variable_constructor(VariableType_Local, frame, 0));
	Symbol z = Symbol_constructor(SymbolType_Variable, "z", Variable_constructor(VariableType_Local, frame, 4));
	StackFrame_add(frame, x);
	StackFrame_add(frame, y);
	StackFrame_add(frame, z);


	assign("x", "y", regSet, frame);

	return 0;
}
