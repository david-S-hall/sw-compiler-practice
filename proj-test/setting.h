#ifndef SETTING_H_INCLUDED
#define SETTING_H_INCLUDED

#include <stdbool.h>

#define N_SYM 57		/* symbol number */
#define N_RW 13			/* reversed words number */
#define N_FCT 11		/* fct code number */
#define LEN_NUM 14		/* max length of a number */
#define LEN_ID 20		/* max length of an identity */
#define LEN_L 100       /* max length of a line */
#define MAX_ERR 50		/* number of the error can be processed at most */
#define MAX_CX 200		/* max number of fct */
#define SIZE_TB 150		/* capacity of symbol table */
#define SIZE_STACK 500	/* capacity of runtime stack */
#define BOUND_ADR 2048	/* bound of address */


/* symbols dict */
typedef enum SYMBOL {
    nul,		ident,		number,     varsym,     letsym,
    funcsym,    plus,		minus,      times,		slash,
    mod,        autoincre,  autodecre,  andsym,     orsym,
    notsym,     bitand,     bitor,      bitnot,     xor,
    shl,        shr,        becomes,    plusbe,     minusbe,
    timesbe,    slashbe,    modbe,      andbe,      orbe,
    xorbe,      shlbe,      shrbe,      eql,        neq,
    lss,        leq,		gtr,        geq,        lparen,
    rparen,     lbrace,		rbrace,     range,      halfrange,
    semicolon,  period,     ifsym,      elsesym,	forsym,
    insym,      whilesym,   repeatsym,  readsym,	printsym,
    callsym,    returnsym,
}SYMBOL;

/* identity types */
typedef enum OBJECT {
    constant,
	variable,
	function,
}OBJECT;

/* fct code types */
typedef enum FCT {
	lit,	opr,	lod,
	sto,	cal,	ini,
	jmp,	jne,    jeq,
    in,     out,
}FCT;

char ERR_TP[80][80];          /* error type dictionary */
char word[N_RW][LEN_ID];		/* reversed words' dict */
char mnemonic[N_FCT][5];		/* fct code dict */
enum SYMBOL ssym[256];			/* value of single-letter-type symbol */
enum SYMBOL wsym[N_RW];			/* value of reversed word symbol */
bool declbegsys[N_SYM];			/* start symbol set of declaration */
bool statbegsys[N_SYM];			/* start symbol set of statement */
bool facbegsys[N_SYM];			/* start symbol set of factor */

#endif // SETTING_H_INCLUDED
