#pragma once
enum RegisterType {
	RegisterType_ZERO,
	RegisterType_AT,
	RegisterType_Values,
	RegisterType_Arguments,
	RegisterType_Temporaries,
	RegisterType_SavedValues,
	RegisterType_GlobalPointer,
	RegisterType_Interuption,
	RegisterType_StackPointer,
	RegisterType_FramePointer,
	RegisterType_ReturnAddress
};
typedef enum RegisterType RegisterType;