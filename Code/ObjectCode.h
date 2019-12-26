#pragma once
#include "IntermediateLanguage.h"
#include "SymbolOps.h"
#include "RegisterSet.h"

#define FAIL 32767

FILE* objectCodeFile;
StackFrame stackFrame;
RegisterSet regSet;
 Symbol baseReg;
struct MyRegs{
    char name[3];
    int avail;
};
typedef struct MyRegs MyRegs;
MyRegs myRegs[3];

void push(int* sp,char* reg, int space){
    if(reg!=NULL)
        fprintf(objectCodeFile,"sw $%s, $sp\n",reg);
    fprintf(objectCodeFile,"addi $sp, $sp, -%d\n",space);
    *sp=*sp-space;
}

int getOffset(char* var){
    Symbol sym=StackFrame_get(stackFrame,var);
    if(sym==NULL)
        return FAIL;
    else
        return Variable_getOffset(Symbol_getVariable(sym));
}

void loginVar(char* var,int offset){
    Symbol sym = Symbol_constructor(SymbolType_Variable, var, 
    Variable_constructor(VariableType_Local, stackFrame, offset));
    StackFrame_add(stackFrame,sym);
}

char* getReg(int* sp,char* var,int isLoad){
    char* ret;
    for(int i=0;i<3;i++)
        if(myRegs[i].avail==1){
            ret=myRegs[i].name;
            myRegs[i].avail=0;
            break;
        }
    if(var==NULL)
        return ret;
    else if(isLoad==1){
        if(var[0]=='&'){
            int offset=getOffset(var+1);
            fprintf(objectCodeFile,"addi $%s, $fp, %d\n",ret,offset);
        }else{
            int offset=getOffset(var);
            fprintf(objectCodeFile,"lw $%s, %d($fp)\n",ret,offset);
        }
        return ret;
    }
    else{
        if(getOffset(var)==FAIL){
            loginVar(var,*sp);
            push(sp,NULL,4);
        }
        return ret;
    }
}

void writeBack(char* reg,char* var){
    int offset=getOffset(var);
    fprintf(objectCodeFile,"sw $%s, %d($fp)\n",reg,offset);
}

int currentArg;

void singleCodeGen(char* target,char* arg1,char* arg2,ILOP op,int* sp){
    //sp用来模拟当前栈顶，栈向下生长，注意这里sp是相对地址，相对当前帧的生长
    for(int i=0;i<3;i++)
        myRegs[i].avail=1;
    char *reg1,*reg2,*reg3;
    int offset;
    switch(op){
        case ILOP_LABEL:
            fprintf(objectCodeFile,"$%s:\n",target);
            break;
        case ILOP_FUNCTION:
            baseReg = RegisterSet_getByName(regSet, "fp");
            stackFrame=StackFrame_constructor(baseReg);
            currentArg=0;
            *sp=0;
            fprintf(objectCodeFile,"$%s:\n",target);
            push(sp,"fp",4);
            break;
        case ILOP_ASSIGN:
            if(arg1[0]!='*'&&target[0]!='*'){          
                if(arg1[0]=='#'){
                    reg1=getReg(sp,target,0);
                    fprintf(objectCodeFile,"li $%s, $%s\n",reg1,arg1+1);
                    writeBack(reg1,target);
                }
                else{
                    reg1=getReg(sp,target,0);
                    reg2=getReg(sp,arg1,1);
                    fprintf(objectCodeFile,"move $%s, $%s\n",reg1,reg2);
                    writeBack(reg1,target);
                }
            }
            else if(arg1[0]=='*'){
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg1+1,1);
                fprintf(objectCodeFile,"lw $%s, 0($%s)\n",reg1,reg2);
                writeBack(reg1,target);
            }
            else if(target[0]=='*'){
                reg1=getReg(sp,target+1,1);
                reg2=getReg(sp,arg1,1);
                fprintf(objectCodeFile,"sw $%s, 0($%s)\n",reg2,reg1);
            }
            break;
        case ILOP_PLUS:
            if(arg1[0]=='#'){
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg2,1);
                fprintf(objectCodeFile,"addi $%s, $%s, %s\n",reg1,reg2,arg1+1);
                writeBack(reg1,target);
            }
            else if(arg2[0]=='#'){
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg1,1);
                fprintf(objectCodeFile,"addi $%s, $%s, %s\n",reg1,reg2,arg2+1);
                writeBack(reg1,target);
            }
            else{
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg1,1);
                reg3=getReg(sp,arg2,1);
                fprintf(objectCodeFile,"add $%s, $%s, $%s\n",reg1,reg2,reg3);
                writeBack(reg1,target);
            }
            break;
        case ILOP_MINUS:
            if(arg1[0]=='#'){
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg2,1);
                reg3=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"li $%s, %s\n",reg3,arg1+1);
                fprintf(objectCodeFile,"sub $%s, $%s, $%s\n",reg1,reg3,reg2);
                writeBack(reg1,target);
            }
            else if(arg2[0]=='#'){
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg1,1);
                fprintf(objectCodeFile,"addi $%s, $%s, -%s\n",reg1,reg2,arg2+1);
                writeBack(reg1,target);
            }
            else{
                reg1=getReg(sp,target,0);
                reg2=getReg(sp,arg1,1);
                reg3=getReg(sp,arg2,0);
                fprintf(objectCodeFile,"sub $%s, $%s, $%s\n",reg1,reg2,reg3);
                writeBack(reg1,target);
            }
            break;
        case ILOP_MUL:
            reg1=getReg(sp,target,0);
            if(arg1[0]=='#')
            {
                reg2=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"li $%s, %s\n",reg2,arg1+1);
            }
            else
                reg2=getReg(sp,arg1,1);
            if(arg2[0]=='#')
            {
                reg3=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"li $%s, %s\n",reg3,arg2+1);
            }
            else
                reg3=getReg(sp,arg1,1);
            fprintf(objectCodeFile,"mul $%s, $%s, $%s\n",reg1,reg2,reg3);
            writeBack(reg1,target);
            break;
        case ILOP_DIV:
            reg1=getReg(sp,target,0);
            if(arg1[0]=='#')
            {
                reg2=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"li $%s, %s\n",reg2,arg1+1);
            }
            else
                reg2=getReg(sp,arg1,1);
            if(arg2[0]=='#')
            {
                reg3=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"li $%s, %s\n",reg3,arg2+1);
            }
            else
                reg3=getReg(sp,arg1,1);
            fprintf(objectCodeFile,"div $%s, $%s\n",reg2,reg3);
            fprintf(objectCodeFile,"mflo $%s\n",reg1);
            writeBack(reg1,target);
            break;
        case ILOP_ADDR:
            reg1=getReg(sp,target,0);
            offset=getOffset(arg1+1);
            fprintf(objectCodeFile,"addi $%s, $fp, %d\n",reg1,offset);
            writeBack(reg1,target);
            break;
        case ILOP_GOTO:
            fprintf(objectCodeFile,"j %s\n",target);
            break;
        case ILOP_IF_G:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"bgt $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_IF_GE:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"bge $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_IF_E:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"beq $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_IF_NE:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"bne $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_IF_L:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"blt $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_IF_LE:
            reg1=getReg(sp,arg1,1);
            reg2=getReg(sp,arg2,1);
            fprintf(objectCodeFile,"ble $%s, $%s, %s\n",reg1,reg2,target);
            break;
        case ILOP_RETURN:
            reg1=getReg(sp,target,1);
            fprintf(objectCodeFile,"move $v0, $%s\n",reg1);
            fprintf(objectCodeFile,"move $sp, $fp\n");//sp=fp，恢复栈顶
            fprintf(objectCodeFile,"lw $fp, 0($fp)\n");//fp=*fp，fp指向的位置存放旧fp
            fprintf(objectCodeFile,"jr $ra\n\n");//注意在call时保存ra
            break;
        case ILOP_DEC:
            loginVar(target,atoi(arg1));
            push(sp,NULL,atoi(arg1));
            break;
        case ILOP_ARG:
            if(target[0]!='&')
                reg1=getReg(sp,target,1);
            else{
                offset=getOffset(target);
                reg1=getReg(sp,NULL,0);
                fprintf(objectCodeFile,"addi $%s, $fp, %d\n",reg1,offset);
            }
            fprintf(objectCodeFile,"sw $%s, %d($fp)\n",reg1,*sp);
            push(sp,reg1,4);
            break;
        case ILOP_PARAM:
            loginVar(target,8+4*currentArg);
            currentArg++;
            break;
        case ILOP_CALL:
            push(sp,"ra",4);
            fprintf(objectCodeFile,"jal %s\n",arg1);
            reg1=getReg(sp,target,0);
            fprintf(objectCodeFile,"move $%s, $v0\n",reg1);
            writeBack(reg1,target);
            fprintf(objectCodeFile,"lw $ra, 4($sp)\n");
            break;
        case ILOP_READ:
            push(sp,"ra",4);
            fprintf(objectCodeFile,"jal read\n");
            reg1=getReg(sp,target,0);
            fprintf(objectCodeFile,"move $%s, $v0\n",reg1);
            writeBack(reg1,target);
            fprintf(objectCodeFile,"lw $ra, 4($sp)\n");
            break;
        case ILOP_WRITE:
            reg1=getReg(sp,target,1);
            fprintf(objectCodeFile,"move $a0, $%s\n",reg1);
            push(sp,"ra",4);
            fprintf(objectCodeFile,"jal write\n");
            fprintf(objectCodeFile,"lw $ra, 4($sp)\n");
            break;
    }
}

void printSomeGarbage(){
    fprintf(objectCodeFile,".data\n");
    fprintf(objectCodeFile,"_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(objectCodeFile,"_ret: .asciiz \"\\n\"\n");
    fprintf(objectCodeFile,"./globl main\n");
    fprintf(objectCodeFile,".text\n");
    fprintf(objectCodeFile,"read:\n");
    fprintf(objectCodeFile,"li $v0, 4\n");
    fprintf(objectCodeFile,"la $a0, _prompt\n");
    fprintf(objectCodeFile,"syscall\n");
    fprintf(objectCodeFile,"jr $ra\n\n");
    fprintf(objectCodeFile,"write:\n");
    fprintf(objectCodeFile,"li $v0, 4\n");
    fprintf(objectCodeFile,"la $a0, _ret\n");
    fprintf(objectCodeFile,"syscall\n");
    fprintf(objectCodeFile,"move $v0, $0\n");
    fprintf(objectCodeFile,"jr $ra\n\n");
}

void objectCodeGen(FILE* file,int base){
    //base表示栈顶初始位置，向下生长（push操作栈顶-4）
    strcpy(myRegs[0].name,"t0");
    strcpy(myRegs[1].name,"t1");
    strcpy(myRegs[2].name,"t2");
    objectCodeFile=file;
    printSomeGarbage();
 
    MyListIterator it = MyList_createIterator(interCodeList);
    while(MyList_hasNext(it)){
        InterCode code=(InterCode)MyList_getNext(it);
        const char* target=code->target;
        const char* arg1=code->arg1;
        const char* arg2=code->arg2;
        singleCodeGen((char*)target,(char*)arg1,(char*)arg2,code->op,&base);
    }
}