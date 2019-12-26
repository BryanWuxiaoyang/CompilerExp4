#pragma once
#include "Type.h"
#include "MyList.h"
struct _Func_ {
	char* name;
	MyListHead paramList;
	Type returnType;
	int state; // 0: unimplemented, 1: implemented
	int decLineno;
	int defLineno;
};
typedef struct _Func_* Func;

Func readFunc;
Func writeFunc;