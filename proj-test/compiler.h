#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include <stdio.h>
#include "setting.h"

#undef __DEBUG__

typedef struct TableStruct
{
    char name[LEN_ID];
    OBJECT kind;
    DATATYPE type;
    int val;
    int level;
    int adr;
    int size;
}TableStruct;

typedef struct Instruction
{
	FCT f;	    // code type
	int l;		// level distance
	int a;		// operation number for different code
}Instruction;

/* global variables for compiler */

unsigned char ch;				// store loaded character for getch
SYMBOL sym;		        // current symbol
char id[LEN_ID+1];		// current identity
int num;				// current number
int cc, ll;				// counter for getch
int cx;					// pointer for virtual machine
char line[LEN_L+1]; 	// line buffer
char a[LEN_ID+1];		// symbol buffer
int line_num;           // counter for read line
int err_num;			// counter for known errors
int fend_tag;           // tag of read the end of file
int rtn_num;            // unbackfilled fct code counter
int rtn_type;           // type of return type

TableStruct table[SIZE_TB];
Instruction code[MAX_CX];

/* output tools */

bool listswitch;		// 1 then shows fct
bool tableswitch;		// 1 then shows symbol table

FILE* fin;
FILE* ftable;
FILE* fcode;
FILE* ferr;
FILE* fresult;
FILE* foutput;

/* function declarations */
void init_proc();
void init_errors();
void init_setting();
void init();

void error(int n);
void getsym();
void getch();
void gen(FCT x, int y, int z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);

void parsing();
void processing();
void interpret();
void debug();
void listcode(int cx0);
void listall();
void problem(int lev, int tx, bool* fsys);
void func_decl_body(int lev, int tx, bool* fsys);
void data_decl(int lev, int* ptx, int* pdx);
void var_decl(int lev, int* ptx, int* pdx);
void let_decl(int lev, int* ptx, int* pdx);
void declaration(OBJECT tp, int* ptx, int lev, int* pdx);
void parameter(bool* fsys, int* ptx, int lev, int p);
void statement(bool* fsys, int* ptx, int lev);
#define expression(fsys, ptx, lev) 	cond_or(fsys, ptx, lev)
void cond_or(bool* fsys, int* ptx, int lev);
void cond_and(bool* fsys, int* ptx, int lev);
void or_expr(bool* fsys, int* ptx, int lev);
void xor_expr(bool* fsys, int* ptx, int lev);
void and_expr(bool* fsys, int* ptx, int lev);
void equal_expr(bool* fsys, int* ptx, int lev);
void rel_expr(bool* fsys, int* ptx, int lev);
void shift_expr(bool* fsys, int* ptx, int lev);
void add_expr(bool* fsys, int* ptx, int lev);
void term(bool* fsys, int* ptx, int lev);
void factor(bool* fsys, int* ptx, int lev);

void enter(OBJECT k, int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
int base(int l, int* s, int b);

#endif // COMPILER_H_INCLUDED
