#pragma once
#include "IntermediateLanguage.h"
#include <stdlib.h>
#include <string.h>

#define INF 0x3fffffff

struct UniNode{
    void* value;
    struct UniNode* next;
    struct UniNode* prev;
};
typedef struct UniNode UniNode;

UniNode* createUniNode(void* value,UniNode* next,UniNode* prev){
    UniNode* newUniNode=malloc(sizeof(UniNode));
    newUniNode->value=value;
    newUniNode->next=next;
    newUniNode->prev=prev;
    return newUniNode;
}

struct UniNodeList{
    UniNode* head;
    UniNode* rear;
};
typedef struct UniNodeList UniNodeList;

UniNodeList* createUniNodeList(){
    UniNodeList* newUniNodeList=malloc(sizeof(UniNodeList));
    newUniNodeList->head=createUniNode(NULL,NULL,NULL);
    newUniNodeList->rear=newUniNodeList->head;
    return newUniNodeList;
}

void addUniNode(UniNodeList* list, void* value){
    //在末尾加入一个节点，并将rear指向加入元素的下一个元素
    UniNode* cur;
    for(cur=list->head;cur!=list->rear;cur=cur->next);
    cur->value=value;
    UniNode* newUnNode=createUniNode(NULL,NULL,cur);
    cur->next=newUnNode;
    list->rear=newUnNode;
}

struct SymChecker{
    char symName[32];
    UniNodeList* assignTable;
    UniNodeList* useTable;
};
typedef struct SymChecker SymChecker;

SymChecker* createSymChecker(char* name){
    SymChecker* newSymChecker=malloc(sizeof(SymChecker));
    strcpy(newSymChecker->symName,name);
    newSymChecker->assignTable=createUniNodeList();
    newSymChecker->useTable=createUniNodeList();
    return newSymChecker;
}

enum TableType{
    USETAB,
    ASSIGNTAB
};
typedef enum TableType TableType;

int searchLine(SymChecker* sym,int lineno, TableType tableType){
    //返回第一个第lineno及其之后引用/赋值该变量的行数，如果没有，返回正无穷
    UniNodeList* list;
    if(tableType==ASSIGNTAB)
        list=sym->assignTable;
    else
        list=sym->useTable;
    for(UniNode* cur=list->head;cur!=list->rear;cur=cur->next)
        if(*(int*)(cur->value)>=lineno)
            return *(int*)(cur->value);
    return INF;
}

int isSameSym(char* name,SymChecker* sym){
    return strcmp(name,sym->symName)==0;
}


SymChecker* searchSymList(char* name,UniNodeList* list){
    if(name[0]=='#')
        return NULL;//立即数不参与优化处理
    if(name[0]=='&'||name[0]=='*')
        name=name+1;
    for(UniNode* cur=list->head;cur!=list->rear;cur=cur->next)
        if(isSameSym(name,cur->value))
            return cur->value;
    return NULL;
}

enum CodePart{
    ARG,
    TARGET
};
typedef enum CodePart CodePart;

void processCodePart(char* name,UniNodeList* symList,int* lineno,CodePart codePart){
    if(name[0]=='#')
        return;//立即数不参与优化处理
    if(name[0]=='&'||name[0]=='*')
        name=name+1;
    SymChecker* symChecker=searchSymList(name,symList);
    if(symChecker==NULL){
        symChecker=createSymChecker(name);
        addUniNode(symList,symChecker);
    }
    if(codePart==ARG)//引用
        addUniNode(symChecker->useTable,lineno);
    else if(codePart==TARGET)//赋值
        addUniNode(symChecker->assignTable,lineno);
}


void processCode(InterCode code,int lineno,UniNodeList* symList){
    //装载各种引用表和赋值表 
    //注：没有处理标签
    //TODO：还缺少指针，标签以及引用
    char* arg1 = code->arg1;
	char* arg2 = code->arg2;
	char* target = code->target;
    int *p=malloc(sizeof(int));
    *p=lineno;
    switch(code->op){
        //下面这些要处理两个参数引用和一个目标赋值
        case ILOP_PLUS:
        case ILOP_MINUS:
        case ILOP_MUL:
        case ILOP_DIV:
            processCodePart(arg1,symList,p,ARG);
            processCodePart(arg2,symList,p,ARG);
            processCodePart(target,symList,p,TARGET);
            break;
        //下面这些要处理一个参数引用和一个目标赋值
        case ILOP_ASSIGN:
        case ILOP_ADDR:
            processCodePart(arg1,symList,p,ARG);
            processCodePart(target,symList,p,TARGET);
            break;
        //下面这些要处理两个参数引用
        case ILOP_IF_G:
        case ILOP_IF_GE:
        case ILOP_IF_E:
        case ILOP_IF_NE:
        case ILOP_IF_L:
        case ILOP_IF_LE:
            processCodePart(arg1,symList,p,ARG);
            processCodePart(arg2,symList,p,ARG);
            break;
        //下面这些要处理一个目标赋值
        case ILOP_DEC:
        case ILOP_CALL:
            processCodePart(target,symList,p,TARGET);
            break;
        //下面这些要处理一个参数引用
        case ILOP_ARG://注意对于ARG指令，引用的其实是target，不是arg
        case ILOP_RETURN://注意对于RETURN指令，引用的其实是target，不是arg
        case ILOP_PARAM://注意对于PARAM指令，引用的其实是target，不是arg
        case ILOP_WRITE://注意对于WRITE指令，引用的其实是target，不是arg
            processCodePart(target,symList,p,ARG);
            break;
        //下面这些暂且啥也不处理
        case ILOP_LABEL:
        case ILOP_FUNCTION:
        case ILOP_GOTO:
        case ILOP_READ://READ函数就不优化了，要不可能读文件读错位
            break;
    }
}


void deleteUselessCode(UniNodeList* symList,ListIterator it){
    //情况1：无效代码，给了个变量赋值或计算，后面压根没用它
    UniNodeList* uselessLine=createUniNodeList();
    for(UniNode* cur=symList->head;cur!=symList->rear;cur=cur->next){
            int *p;
            SymChecker* curSymChecker=(SymChecker*)(cur->value);
            int curAssignOp,nextUseOp,nextAssignOp;
            for(UniNode* assign=curSymChecker->assignTable->head;assign->next!=curSymChecker->assignTable->rear;assign=assign->next){
                curAssignOp=*(int*)(assign->value);
                nextUseOp=searchLine(curSymChecker,curAssignOp,USETAB);
                nextAssignOp=*(int*)(assign->next->value);
                if(nextUseOp>nextAssignOp){
                    p=malloc(sizeof(int));
                    *p=curAssignOp;
                    addUniNode(uselessLine,p);
                }
            }
            nextUseOp=searchLine(curSymChecker,nextAssignOp,USETAB);//处理最后一个赋值
            if(nextUseOp>=INF){
                p=malloc(sizeof(int));
                *p=nextAssignOp;
                addUniNode(uselessLine,p);
            }
    }
    int cur=0;
    UniNode* nextDeleteLineNode=uselessLine->head;
	while (MyList_hasNext(it)) {
        if(cur==*(int*)(nextDeleteLineNode->value)){
            MyList_removeNext(it);
            nextDeleteLineNode=nextDeleteLineNode->next;
        }   
		MyList_getNext(it);
        cur++;
	}
}

void deleteUselessCode2(UniNodeList* symList,ListIterator it){
    UniNodeList* uselessLine=createUniNodeList();
    int lineno=0;
    while(MyList_hasNext(it)){
        lineno++;
        InterCode code=(InterCode)MyList_getNext(it);
        char* target = code->target;
        char* arg1 = code->arg1;
	    char* arg2 = code->arg2;
        if(target[0]=='&'||target[0]=='*')
            continue;
        if(code->op==ILOP_ASSIGN||code->op==ILOP_PLUS||code->op==ILOP_MINUS||code->op==ILOP_MUL||code->op==ILOP_DIV)
        {
            SymChecker* symTarget=searchSymList(target,symList);
            if(searchLine(symTarget,lineno,USETAB)>=INF)
                MyList_removePrev(it);
        }
    }
}

void processUselessCode(UniNodeList* symList,ListIterator it){
    //情况2：无效中间变量，给变量t赋值为a，a在后面没被修改，用t参与计算或赋值
    //情况3：类似情况2，给变量t计算为一个东西，参与计算的变量在后面没被修改，又把t赋值给另一个变量
    int lineno=0;
    while(MyList_hasNext(it)){
        InterCode code=(InterCode)MyList_getNext(it);
        InterCode curCode;
        char* arg1 = code->arg1;
	    char* arg2 = code->arg2;
	    char* target = code->target;
        int changeLineArg,changeLineTarget,changeLine;
        int i;
        SymChecker* arg1Sym;
        SymChecker* targetSym;
        ListIterator runIt=it;
        switch(code->op){
            case ILOP_ASSIGN:
                targetSym=searchSymList(target,symList);
                arg1Sym=searchSymList(arg1,symList);
                changeLineArg=searchLine(arg1Sym,lineno,ASSIGNTAB);
                changeLineTarget=searchLine(targetSym,lineno,ASSIGNTAB);
                changeLine=changeLineTarget<changeLineArg?changeLineTarget:changeLineArg;
                if(changeLine>=INF)
                    changeLine=-1;
                for(i=lineno+1;i<=changeLine;i++){
                    curCode=(InterCode)MyList_getNext(runIt);
                    if(strcmp(curCode->arg1,target)==0)
                        strcpy(curCode->arg1,target);
                    if(strcmp(curCode->arg2,target)==0)
                        strcpy(curCode->arg2,target);
                }
            }
        lineno++;
    }
}

void processUselessCode2(ListIterator it,UniNodeList* symList){
    int lineno=0;
    while(MyList_hasNext(it)){
        InterCode code=(InterCode)MyList_getNext(it);
	    char* target = code->target;
        char* arg1=code->arg1;
        if(code->op==ILOP_CALL||code->op==ILOP_PLUS||code->op==ILOP_MINUS||code->op==ILOP_MUL||code->op==ILOP_DIV){
            InterCode nextCode=(InterCode)MyList_getNext(it);
            char* nextArg1=nextCode->arg1;
            char* nextTarget=nextCode->target;
            SymChecker* symTarget=searchSymList(target,symList);
            int nextUse=searchLine(symTarget,lineno+2,USETAB);
            if(nextCode->op==ILOP_ASSIGN&&nextUse>=INF&&strcmp(target,nextArg1)==0&&nextArg1[0]!='*'&&nextArg1[0]!='&'&&nextTarget[0]!='*'&&nextTarget[0]!='&'){
                strcpy(target,nextTarget);
                MyList_removePrev(it);
            }
            lineno++;
        }
        if(code->op==ILOP_RETURN){
            MyList_getPrev(it);
            InterCode prevCode=(InterCode)MyList_getPrev(it);
            char* prevArg1=prevCode->arg1;
            char* prevTarget=prevCode->target;
            if(prevCode->op==ILOP_ASSIGN&&strcmp(target,prevTarget)==0&&prevArg1[0]!='*'&&prevArg1[0]!='&'&&prevTarget[0]!='*'&&prevTarget[0]!='&'){
                strcpy(target,prevArg1);
                MyList_removeNext(it);
            }
            else
                MyList_getNext(it);
            MyList_getNext(it);
        }
        lineno++;
    }
}

UniNodeList* getCodeList(ListIterator it,int lineno){
    UniNodeList* symList=createUniNodeList();
    int cur=0;
	while (MyList_hasNext(it)&&cur<lineno) {
		InterCode code = (InterCode)MyList_getNext(it);
        processCode(code,cur,symList);
        cur++;
	}
    return symList;
}

struct BaseBlock{
    UniNodeList* codeList;
    UniNodeList* blockOut;
    char* label;
};
typedef struct BaseBlock BaseBlock;

enum BlockType{
    LABEL_BLOCK,
    NOLABEL_BLOCK
};
typedef enum BlockType BlockType;

BaseBlock* createBaseBlock(char* label,BlockType blockType){
    BaseBlock* newBaseBlock=malloc(sizeof(BaseBlock));
    newBaseBlock->codeList=createUniNodeList();
    newBaseBlock->blockOut=createUniNodeList();
    if(blockType==LABEL_BLOCK)
        strcpy(newBaseBlock->label,"L_");
    else
        strcpy(newBaseBlock->label,"N_");
    strcat(newBaseBlock->label,label);
    return newBaseBlock;
}

int isSameBlock(char* label,BlockType blockType,BaseBlock* block){
    char name[32];
    if(blockType==LABEL_BLOCK)
        strcpy(name,"L_");
    else
        strcpy(name,"N_");
    strcat(name,label);
    return strcmp(name,block->label)==0;
}

BaseBlock* searchBaseBlock(char* label,BlockType blockType,UniNodeList* blockList){
    for(UniNode* cur=blockList->head;cur!=blockList->rear;cur=cur->next)
        if(isSameBlock(label,blockType,cur->value))
            return cur->value;
    return NULL;
}

void optimizeBlocks(UniNodeList* blockList){
    //针对一个函数的所有块
    for(UniNode* cur=blockList->head;cur!=blockList->rear;cur=cur->next){
        BaseBlock* curBlock=(BaseBlock*)(cur->value);
        InterCode lastCode=curBlock->codeList->rear->prev;
        InterCode prevCode=curBlock->codeList->rear->prev->prev;
        if(lastCode!=NULL&&lastCode->op==ILOP_GOTO){
            BaseBlock* gotoBlock=searchBaseBlock(lastCode->target,LABEL_BLOCK,blockList);
            addUniNode(curBlock->blockOut,gotoBlock);
            if(prevCode!=NULL&&(prevCode->op==ILOP_IF_G
                ||prevCode->op ==ILOP_IF_GE
                ||prevCode->op== ILOP_IF_E
                ||prevCode->op== ILOP_IF_NE
                ||prevCode->op== ILOP_IF_L
                ||prevCode->op== ILOP_IF_LE))
                addUniNode(curBlock->blockOut,cur->next->value);
        }
    }
}

UniNodeList* getBlockList(ListIterator it){
    UniNodeList* blockList;
    UniNodeList* allBlockList=createUniNodeList();
    BaseBlock* curBlock;
    int lineno=0;
    while(MyList_hasNext(it)){
        InterCode code=(InterCode)MyList_getNext(it);
        char* target = code->target;
        if(code->op==ILOP_GOTO){
            InterCode nextCode=(InterCode)MyList_getNext(it);
            MyList_getPrev(it);
            addUniNode(curBlock->codeList,code);
            if(nextCode->op!=ILOP_LABEL){
                char label[32];
                sprintf(label,"%d",lineno+1);
                curBlock=createBaseBlock(label,NOLABEL_BLOCK);
                addUniNode(blockList,curBlock);
            }
        }
        else if(code->op==ILOP_LABEL){
            curBlock=createBaseBlock(target,LABEL_BLOCK);
            addUniNode(blockList,curBlock);
            addUniNode(curBlock->codeList,code);
        }
        else if(code->op==ILOP_FUNCTION){
            blockList=createUniNodeList();
            addUniNode(allBlockList,blockList);
            char label[32];
            sprintf(label,"%d",lineno);
            curBlock=createBaseBlock(label,NOLABEL_BLOCK);
            addUniNode(blockList,curBlock);
            addUniNode(curBlock->codeList,code);
        }
        else
            addUniNode(curBlock->codeList,code);
        lineno++;
    }
    for(UniNode* cur=allBlockList->head;cur!=allBlockList->rear;cur=cur->next)
        optimizeBlocks(cur->value);
    return allBlockList;
}

ListIterator searchLabelList(char* name,UniNodeList* list){
    for(UniNode* cur=list->head;cur!=list->rear;cur=cur->next){
        ListIterator it=(ListIterator)(cur->value);
        InterCode code=(InterCode)MyList_peekNext(it);
        if(strcmp(name,code->target)==0)
            return cur->value;
    }
    return NULL;
}

void processLoop(ListIterator it){
    //it指向label之前的位置
    ListIterator it2=MyList_copyIterator(it);
    InterCode label=(InterCode)MyList_peekNext(it2);
    char* labelName=label->target;
    int lineno=0;
    while(MyList_hasNext(it2)){
        lineno++;
        InterCode code=(InterCode)MyList_getNext(it2);
        if(code->op==ILOP_GOTO&&strcmp(code->target,labelName)==0)
            break;
    }
    it2=MyList_copyIterator(it);
    UniNodeList* symList=getCodeList(it2,lineno);
    it2=MyList_copyIterator(it);
    lineno=0;
    while(MyList_hasNext(it2)){
        InterCode code=(InterCode)MyList_getNext(it2);
        lineno++;
        if(code->op==ILOP_GOTO&&strcmp(code->target,labelName)==0)
            break;
        if(code->op==ILOP_PLUS||code->op==ILOP_MINUS||code->op==ILOP_MUL||code->op==ILOP_DIV||code->op==ILOP_ASSIGN){
            char* target=code->target;
            char* arg1=code->arg1;
            char* arg2=code->arg2;
            if((arg1!=NULL&&strcmp(target,arg1)==0)||(arg2!=NULL&&strcmp(target,arg2)==0))
                continue;
            if(target[0]=='&'||target[0]=='*')
                continue;
            SymChecker* symTarget=searchSymList(target,symList);
            int assignCnt=0;
            for(UniNode* cur=symTarget->assignTable->head;cur!=symTarget->assignTable->rear;cur=cur->next)
                assignCnt++;
            if(assignCnt>1)
                continue;
            if(searchLine(symTarget,0,USETAB)<lineno)
                continue;
            assignCnt=0;
            if(arg1[0]=='#'||arg1[0]=='&'||arg1[0]=='*')
                continue;
            SymChecker* symArg1=searchSymList(arg1,symList);
            for(UniNode* cur=symArg1->assignTable->head;cur!=symArg1->assignTable->rear;cur=cur->next)
                assignCnt++;
            if(assignCnt>0)
                continue;
            if(code->op!=ILOP_ASSIGN){
                assignCnt=0;
                if(arg2[0]=='#'||arg2[0]=='&'||arg2[0]=='*')
                    continue;
                SymChecker* symArg2=searchSymList(arg2,symList);
                for(UniNode* cur=symArg2->assignTable->head;cur!=symArg2->assignTable->rear;cur=cur->next)
                    assignCnt++;
                if(assignCnt>0)
                    continue;
            }
            MyList_removePrev(it2);
            MyList_insert(it,code);
            MyList_getNext(it);
        }
    }
}

void optimizeLoop(ListIterator it){
    int lineno=0;
    UniNodeList* labelList=createUniNodeList();
    while(MyList_hasNext(it)){
        InterCode code=(InterCode)MyList_getNext(it);
        char* target = code->target;
        if(code->op==ILOP_LABEL){
            ListIterator it2=MyList_copyIterator(it);
            MyList_getPrev(it2);
            addUniNode(labelList,it2);
        }
        else if(code->op==ILOP_GOTO){
            ListIterator gotoLabel=searchLabelList(target,labelList);
            if(gotoLabel!=NULL)
                processLoop(gotoLabel);
        }
        lineno++;
    }
}

void optimizeInterCodeLinear(){	
    ListIterator it = MyList_createIterator(interCodeList);
    optimizeLoop(it);
    it=MyList_createIterator(interCodeList);
    //processUselessCode(symList,it);//处理情况2和3，过程中不会删除代码，只会修改，处理之后会产生一些无效的赋值或计算；但是这样没有考虑循环和分支，有不少问题
    UniNodeList* symList=getCodeList(it,INF);
    it=MyList_createIterator(interCodeList);
    processUselessCode2(it,symList);
    //it=MyList_createIterator(interCodeList);
    //symList=getCodeList(it,INF);
    //it=MyList_createIterator(interCodeList);
    //deleteUselessCode2(symList,it);
    //it=MyList_createIterator(interCodeList);
    //UniNodeList* newSymList=getCodeList(it);
    //it=MyList_createIterator(interCodeList);
    //deleteUselessCode(newSymList,it);//此时统一删除所有无效代码
    //UniNodeList* allBlockList=getBlockList(it);
	MyList_destroyIterator(it);
}
