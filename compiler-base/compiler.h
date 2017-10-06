#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "setting.h"
#include "errortypes.h"

struct TableStruct{
    char name[LEN_ID];
    enum OBJECT kind;
    int val;
    int level;
    int adr;
    int size;
}table[SIZE_TB];


struct Instruction
{
	enum FCT f;	// code type
	int l;		// level distance
	int a;		// operation number for different code
}code[MAX_CX];

/* global variables for compiler */

char ch;				// store loaded character for getch
enum SYMBOL sym;		// current symbol
char id[LEN_ID+1];		// current identity
int num;				// current number
int cc, ll;				// counter for getch
int cx;					// pointer for virtual machine
char line[LEN_L+1]; 	// line buffer
char a[LEN_ID+1];		// symbol buffer
int line_num;           // counter for read line
int err_num;			// counter for known errors

/* output tools */

bool listswitch;		// 1 then shows fct
bool tableswitch;		// 1 then shows symbol table

FILE* fin;
FILE* ftable;
FILE* fcode;
FILE* ferr;
FILE* fresult;

/* function declarations */

void init();
void error(int n);
void getsym();
void getch();
void gen(enum FCT x, int y, int z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
void interpret();
void listcode(int cx0);
void listall();
void problem(int lev, int tx, bool* fsys);
void declaration(enum OBJECT tp, int* ptx, int lev, int* pdx);
void statement(bool* fsys, int* ptx, int lev);
void condition(bool* fsys, int* ptx, int lev);
void expression(bool* fsys, int* ptx, int lev);
void term(bool* fsys, int* ptx, int lev);
void factor(bool* fsys, int* ptx, int lev);
void enter(enum OBJECT k, int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
int base(int l, int* s, int b);

void init_proc()
{
	err_num = 0;
	line_num = 0;
	cc = ll = cx = 0;
	ch = ' ';
}

#endif
