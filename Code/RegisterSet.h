#pragma once
#include "Symbol.h"
#include "SymbolOps.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct _RegisterSet_ {
	Symbol regs[32];
};
typedef struct _RegisterSet_* RegisterSet;

RegisterSet RegisterSet_constructor() {
	RegisterSet regSet = (RegisterSet)malloc(sizeof(struct _RegisterSet_));
	if (regSet != NULL) {
		regSet->regs[0] = Symbol_constructor(SymbolType_Register, "zero", Register_constructor(RegisterType_ZERO));
		regSet->regs[1] = Symbol_constructor(SymbolType_Register, "at", Register_constructor(RegisterType_AT));
		regSet->regs[2] = Symbol_constructor(SymbolType_Register, "v0", Register_constructor(RegisterType_Values));
		regSet->regs[3] = Symbol_constructor(SymbolType_Register, "v1", Register_constructor(RegisterType_Values));
		regSet->regs[4] = Symbol_constructor(SymbolType_Register, "a0", Register_constructor(RegisterType_Arguments));
		regSet->regs[5] = Symbol_constructor(SymbolType_Register, "a1", Register_constructor(RegisterType_Arguments));
		regSet->regs[6] = Symbol_constructor(SymbolType_Register, "a2", Register_constructor(RegisterType_Arguments));
		regSet->regs[7] = Symbol_constructor(SymbolType_Register, "a3", Register_constructor(RegisterType_Arguments));
		regSet->regs[8] = Symbol_constructor(SymbolType_Register, "t0", Register_constructor(RegisterType_Temporaries));
		regSet->regs[9] = Symbol_constructor(SymbolType_Register, "t1", Register_constructor(RegisterType_Temporaries));
		regSet->regs[10] = Symbol_constructor(SymbolType_Register, "t2", Register_constructor(RegisterType_Temporaries));
		regSet->regs[11] = Symbol_constructor(SymbolType_Register, "t3", Register_constructor(RegisterType_Temporaries));
		regSet->regs[12] = Symbol_constructor(SymbolType_Register, "t4", Register_constructor(RegisterType_Temporaries));
		regSet->regs[13] = Symbol_constructor(SymbolType_Register, "t5", Register_constructor(RegisterType_Temporaries));
		regSet->regs[14] = Symbol_constructor(SymbolType_Register, "t6", Register_constructor(RegisterType_Temporaries));
		regSet->regs[15] = Symbol_constructor(SymbolType_Register, "t7", Register_constructor(RegisterType_Temporaries));
		regSet->regs[16] = Symbol_constructor(SymbolType_Register, "s0", Register_constructor(RegisterType_SavedValues));
		regSet->regs[17] = Symbol_constructor(SymbolType_Register, "s1", Register_constructor(RegisterType_SavedValues));
		regSet->regs[18] = Symbol_constructor(SymbolType_Register, "s2", Register_constructor(RegisterType_SavedValues));
		regSet->regs[19] = Symbol_constructor(SymbolType_Register, "s3", Register_constructor(RegisterType_SavedValues));
		regSet->regs[20] = Symbol_constructor(SymbolType_Register, "s4", Register_constructor(RegisterType_SavedValues));
		regSet->regs[21] = Symbol_constructor(SymbolType_Register, "s5", Register_constructor(RegisterType_SavedValues));
		regSet->regs[22] = Symbol_constructor(SymbolType_Register, "s6", Register_constructor(RegisterType_SavedValues));
		regSet->regs[23] = Symbol_constructor(SymbolType_Register, "s7", Register_constructor(RegisterType_SavedValues));
		regSet->regs[24] = Symbol_constructor(SymbolType_Register, "t8", Register_constructor(RegisterType_Temporaries));
		regSet->regs[25] = Symbol_constructor(SymbolType_Register, "t9", Register_constructor(RegisterType_Temporaries));
		regSet->regs[26] = Symbol_constructor(SymbolType_Register, "k0", Register_constructor(RegisterType_Interuption));
		regSet->regs[27] = Symbol_constructor(SymbolType_Register, "k1", Register_constructor(RegisterType_Interuption));
		regSet->regs[28] = Symbol_constructor(SymbolType_Register, "gp", Register_constructor(RegisterType_GlobalPointer));
		regSet->regs[29] = Symbol_constructor(SymbolType_Register, "sp", Register_constructor(RegisterType_StackPointer));
		regSet->regs[30] = Symbol_constructor(SymbolType_Register, "fp", Register_constructor(RegisterType_FramePointer));
		regSet->regs[31] = Symbol_constructor(SymbolType_Register, "ra", Register_constructor(RegisterType_ReturnAddress));
	}
	return regSet;
}

Symbol RegisterSet_getByNo(RegisterSet regSet, int no) {
	return regSet->regs[no];
}

Symbol RegisterSet_getByName(RegisterSet regSet, const char* name) {
	Symbol result = NULL;
	for (int i = 0; i < 32; i++) {
		Symbol reg = regSet->regs[i];
		if (strcmp(reg->name, name) == 0) {
			result = reg;
			break;
		}
	}
	return result;
}

Symbol RegisterSet_allocWithForbid(RegisterSet regSet, Symbol variable, List forbidList) {
	assert(Symbol_getType(variable) == SymbolType_Variable);
	Symbol result = NULL;

	for (int i = 16; i <= 23; i++) {
		Symbol reg = regSet->regs[i];
		if (forbidList != NULL && List_contains(forbidList, reg) == 1) continue;

		if (List_isEmpty(Symbol_getReferenceList(reg)) == 1) {
			result = reg;
			break;
		}
		if (List_contains(Symbol_getReferenceList(reg), variable) == 1) {
			result = reg;
			break;
		}
	}
	if (result != NULL) return result;
	Symbol reg = regSet->regs[16];
	SymbolOps_checkAndModifySelfReference(reg);
	Symbol_clearLinks(reg);
	result = reg;

	return result;
}


Symbol RegisterSet_alloc(RegisterSet regSet, Symbol variable) {
	return RegisterSet_allocWithForbid(regSet, variable, NULL);
}

