#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#define int long long

int *text,		// text segment
	*old_text, // for dump text segment 
	*stack;    //stack

char *data;    // data segment

int *pc, *bp, *sp, ax, cycle; // virtural machine registers

enum{LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT }; //instructions

int token; // current token
char *src, *old_src;
int poolsize;
int line;



void next(){
	token = *src++;// token = *src; src++;
	return;
} //词法分析

void expression(int level) {

} //解释表达式

void program(){
	next(); //get next token
	while (token > 0) {
		printf("token is: %c\n", token);
		next();
	}
} //语法分析


int eval() {
	int op, *tmp;
	while (1) {
		op = *pc++; // get next operation code
		if    (op == IMM)      {ax = *pc++;}
		else if(op == LC)   {ax = *(char *)ax;}
		else if(op == LI)   {ax = *(int *)ax;}
		else if(op == SC)   {ax = *(char *)*sp++ = ax;} // *sp++ 相当于退栈 POP操作
		else if(op == SI)   {*(int *)*sp++ = ax;}
		else if(op == PUSH) {*--sp = ax;}
		else if(op == JMP)  {pc = (int *)*pc;}
		else if(op == JZ)   {pc = ax ? pc + 1 : (int*)*pc;}
		else if(op == JNZ)  {pc = ax ? (int*)*pc : pc + 1;}
		else if(op == CALL) {*--sp = (int)(pc+1); pc = (int*)pc;}
		else if(op == ENT)  {*--sp = (int)bp; bp = sp; sp = sp - *pc++;} //make new stack frame
		else if(op == ADJ)  {sp = sp + *pc++;}
		else if(op == LEV)  {sp = bp; bp = (int*)*sp++; pc = (int*)*sp++;} //restore call frame and PC
		else if(op == LEA)  {ax = (int)(bp + *pc++);}
		else if(op == OR)   ax = *sp++ | ax;
		else if(op == XOR)  ax = *sp++ ^ ax;
		else if(op == AND)  ax = *sp++ & ax;
		else if(op == EQ)   ax = *sp++ == ax;
		else if(op == NE)   ax = *sp++ != ax;
		else if(op == LT)   ax = *sp++ < ax;
		else if(op == LE)   ax = *sp++ <= ax;
		else if(op == GT)   ax = *sp++ > ax;
		else if(op == GE)   ax = *sp++ >= ax;
		else if(op == SHL)  ax = *sp++ << ax;
		else if(op == SHR)  ax = *sp++ >> ax;
		else if(op == ADD)  ax = *sp++ + ax;
		else if(op == SUB)  ax = *sp++ - ax;
		else if(op == MUL)  ax = *sp++ * ax;
		else if(op == DIV)  ax = *sp++ / ax;
		else if(op == MOD)  ax = *sp++ % ax;
		else if(op == EXIT) {printf("exit(%d)", *sp); return *sp;}
		else if(op == OPEN) {ax = open((char*)sp[1], sp[0]);}
		else if(op == CLOS) {ax = close(*sp);}
		else if(op == READ) {ax = read(sp[2], (char*)sp[1], *sp);}
		else if(op == PRTF) {tmp = sp +pc[1]; ax = printf((char*)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]);}
		else if(op == MALC) {ax = (int)malloc(*sp);}
		else if(op == MSET) {ax = (int)memset((char*)sp[2], sp[1], *sp);}
		else if(op == MCMP) {ax = memcmp((char*)sp[2], (char*)sp[1], *sp);}
		else{
			printf("unknown instruction: %d\n", op);
			return -1;
		}
	}	

	return 0;
} //虚拟机入口，解释目标代


int main(int argc, char **argv) {
	int i, fd;

	argc--;
	argv++;

	poolsize = 256 * 1024; //arbitary size
        line = 0;
	
	if((fd = open(*argv, 0)) < 0) {
		printf("could not open(%s)\n", *argv);
		return -1;
	}
	

	if(!(src = old_src = malloc(poolsize))){
			printf("could not malloc(%d) for source asrea\n", poolsize);
			return -1;
	}

	if ((i = read(fd, src, poolsize-1)) <= 0) {
		printf("read() return %d\n", i);
		return -1;
	}

	src[i] = 0;  //add EOF character
	close(fd);

	//allocate memory for virtual machine
	if(!(text = old_text = malloc(poolsize))) {
		printf("could not malloc(%d) for text area\n", poolsize);
		return -1;
	}

	if(!(data = malloc(poolsize))) {
		printf("could not malloc(%d) for data area\n", poolsize);
		return -1;
	}

	if(!(stack = malloc(poolsize))) {
		printf("could not malloc(%d) for stack area\n", poolsize);
		return -1;
	}

	memset(text, 0, poolsize);
	memset(stack, 0, poolsize);
	memset(data, 0, poolsize);


	bp = sp = (int*)((int)stack + poolsize);
	ax = 0;

	i = 0;
    text[i++] = IMM;
    text[i++] = 10;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 20;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;

    pc = text;
	
	program();
	return eval();
}

	




		




