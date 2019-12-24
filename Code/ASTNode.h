#pragma once
#include <assert.h>
#include "MyList.h"
#include "Type.h"

struct _ASTNodeHandler_;

enum ASTNodeType {
	AST_INTEGER,
	AST_FLOAT,
	AST_SYM,
	AST_FUNC,
	AST_OP
};
typedef enum ASTNodeType ASTNodeType;

int isInstantType(ASTNodeType type) {
	return type == AST_INTEGER || type == AST_FLOAT;
}

union ASTNodeValue {
	Sym sym;
	Func func;
	int intValue;
	float floatValue;
	enum OP op;
};
typedef union ASTNodeValue ASTNodeValue;

struct _ASTNode_ {
	int height;

	enum ASTNodeType type;
	union ASTNodeValue value;
	char* name;
	Type varType;

	struct _ASTNode_* lc;
	struct _ASTNode_* rc;

	ListHead parents; 
	ConstValue constValue;  

	int accessTag;
	int removeTag;
};
typedef struct _ASTNode_* ASTNode;

void clearASTNode(ASTNode node) {
	free(node->name);
	MyList_destroyList(node->parents);
}

