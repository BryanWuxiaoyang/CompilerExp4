#pragma once
#include "Symbal.h"
#include "SymbalOps.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct _RegisterSet_ {
	Symbal regs[32];
};
typedef struct _RegisterSet_* RegisterSet;

RegisterSet RegisterSet_constructor() {
	RegisterSet regSet = (RegisterSet)malloc(sizeof(struct _RegisterSet_));
	if (regSet != NULL) {
		regSet->regs[0] = Symbal_constructor(SymbalType_Register, "zero", Register_constructor(RegisterType_ZERO));
		regSet->regs[1] = Symbal_constructor(SymbalType_Register, "at", Register_constructor(RegisterType_AT));
		regSet->regs[2] = Symbal_constructor(SymbalType_Register, "v0", Register_constructor(RegisterType_Values));
		regSet->regs[3] = Symbal_constructor(SymbalType_Register, "v1", Register_constructor(RegisterType_Values));
		regSet->regs[4] = Symbal_constructor(SymbalType_Register, "a0", Register_constructor(RegisterType_Arguments));
		regSet->regs[5] = Symbal_constructor(SymbalType_Register, "a1", Register_constructor(RegisterType_Arguments));
		regSet->regs[6] = Symbal_constructor(SymbalType_Register, "a2", Register_constructor(RegisterType_Arguments));
		regSet->regs[7] = Symbal_constructor(SymbalType_Register, "a3", Register_constructor(RegisterType_Arguments));
		regSet->regs[8] = Symbal_constructor(SymbalType_Register, "t0", Register_constructor(RegisterType_Temporaries));
		regSet->regs[9] = Symbal_constructor(SymbalType_Register, "t1", Register_constructor(RegisterType_Temporaries));
		regSet->regs[10] = Symbal_constructor(SymbalType_Register, "t2", Register_constructor(RegisterType_Temporaries));
		regSet->regs[11] = Symbal_constructor(SymbalType_Register, "t3", Register_constructor(RegisterType_Temporaries));
		regSet->regs[12] = Symbal_constructor(SymbalType_Register, "t4", Register_constructor(RegisterType_Temporaries));
		regSet->regs[13] = Symbal_constructor(SymbalType_Register, "t5", Register_constructor(RegisterType_Temporaries));
		regSet->regs[14] = Symbal_constructor(SymbalType_Register, "t6", Register_constructor(RegisterType_Temporaries));
		regSet->regs[15] = Symbal_constructor(SymbalType_Register, "t7", Register_constructor(RegisterType_Temporaries));
		regSet->regs[16] = Symbal_constructor(SymbalType_Register, "s0", Register_constructor(RegisterType_SavedValues));
		regSet->regs[17] = Symbal_constructor(SymbalType_Register, "s1", Register_constructor(RegisterType_SavedValues));
		regSet->regs[18] = Symbal_constructor(SymbalType_Register, "s2", Register_constructor(RegisterType_SavedValues));
		regSet->regs[19] = Symbal_constructor(SymbalType_Register, "s3", Register_constructor(RegisterType_SavedValues));
		regSet->regs[20] = Symbal_constructor(SymbalType_Register, "s4", Register_constructor(RegisterType_SavedValues));
		regSet->regs[21] = Symbal_constructor(SymbalType_Register, "s5", Register_constructor(RegisterType_SavedValues));
		regSet->regs[22] = Symbal_constructor(SymbalType_Register, "s6", Register_constructor(RegisterType_SavedValues));
		regSet->regs[23] = Symbal_constructor(SymbalType_Register, "s7", Register_constructor(RegisterType_SavedValues));
		regSet->regs[24] = Symbal_constructor(SymbalType_Register, "t8", Register_constructor(RegisterType_Temporaries));
		regSet->regs[25] = Symbal_constructor(SymbalType_Register, "t9", Register_constructor(RegisterType_Temporaries));
		regSet->regs[26] = Symbal_constructor(SymbalType_Register, "k0", Register_constructor(RegisterType_Interuption));
		regSet->regs[27] = Symbal_constructor(SymbalType_Register, "k1", Register_constructor(RegisterType_Interuption));
		regSet->regs[28] = Symbal_constructor(SymbalType_Register, "gp", Register_constructor(RegisterType_GlobalPointer));
		regSet->regs[29] = Symbal_constructor(SymbalType_Register, "sp", Register_constructor(RegisterType_StackPointer));
		regSet->regs[30] = Symbal_constructor(SymbalType_Register, "fp", Register_constructor(RegisterType_FramePointer));
		regSet->regs[31] = Symbal_constructor(SymbalType_Register, "ra", Register_constructor(RegisterType_ReturnAddress));
	}
	return regSet;
}

Symbal RegisterSet_getByNo(RegisterSet regSet, int no) {
	return regSet->regs[no];
}

Symbal RegisterSet_getByName(RegisterSet regSet, const char* name) {
	Symbal result = NULL;
	for (int i = 0; i < 32; i++) {
		Symbal reg = regSet->regs[i];
		if (strcmp(reg->name, name) == 0) {
			result = reg;
			break;
		}
	}
	return result;
}

Symbal RegisterSet_allocWithForbid(RegisterSet regSet, Symbal variable, List forbidList) {
	assert(Symbal_getType(variable) == SymbalType_Variable);
	Symbal result = NULL;

	for (int i = 16; i <= 23; i++) {
		Symbal reg = regSet->regs[i];
		if (forbidList != NULL && List_contains(forbidList, reg) == 1) continue;

		if (List_isEmpty(Symbal_getReferenceList(reg)) == 1) {
			result = reg;
			break;
		}
		if (List_contains(Symbal_getReferenceList(reg), variable) == 1) {
			result = reg;
			break;
		}
	}
	if (result != NULL) return result;
	Symbal reg = regSet->regs[16];
	SymbalOps_checkAndModifySelfReference(reg);
	Symbal_clearLinks(reg);
	result = reg;

	return result;
}


Symbal RegisterSet_alloc(RegisterSet regSet, Symbal variable) {
	return RegisterSet_allocWithForbid(regSet, variable, NULL);
}

