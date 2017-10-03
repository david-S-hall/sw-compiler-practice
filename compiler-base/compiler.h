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
};

FILE* fin;
FILE* ftable;
FILE* fcode;
FILE* ferr;
FILE* fresult;

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

void init();
void error(int n);
void getsym();
void getch();
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
void test(bool* s1, bool* s2, int n);
void block(int lev, int tx, bool* fsys);
void declaration(enum OBJECT tp, int* ptx, int lev, int* pdx);

void init_proc()
{
	err_num = 0;
	line_num = 0;
	cc = ll = cx = 0;
	ch = ' ';
}

#endif
