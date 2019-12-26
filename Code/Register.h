#pragma once
#include "RegisterType.h"
#include <stdlib.h>
struct _Register_ {
	RegisterType type;
};
typedef struct _Register_* Register;

Register Register_constructor(RegisterType type) {
	Register reg = (Register)malloc(sizeof(struct _Register_));
	if (reg != NULL) {
		reg->type = type;
	}
	return reg;
}

RegisterType Register_getType(Register reg) {
	return reg->type;
}