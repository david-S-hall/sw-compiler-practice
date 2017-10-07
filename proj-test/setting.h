#ifndef SETTING_H_INCLUDED
#define SETTING_H_INCLUDED

#include <stdbool.h>

#define N_SYM 31		/* symbol number */
#define N_RW 10			/* reversed words number */
#define N_FCT 8			/* fct code number */
#define LEN_NUM 14		/* max length of a number */
#define LEN_ID 16		/* max length of an identity */
#define LEN_L 100       /* max length of a line */
#define MAX_ERR 30		/* number of the error can be processed at most */
#define MAX_CX 200		/* max number of fct */
#define SIZE_TB 100		/* capacity of symbol table */
#define SIZE_STACK 500	/* capacity of runtime stack */
#define BOUND_ADR 2048	/* bound of address */


/* symbols dict */
typedef enum SYMBOL {
    nul,		ident,		number, 	plus,		minus,
    times,		slash,		becomes,	eql,		neq,
    lss,		leq,		gtr,		geq,		lparen,
    rparen,		lbrace,		rbrace,		range,		semicolon,
    ifsym,		elsesym,	forsym,		insym,		whilesym,
    readsym,	printsym,	callsym,	varsym,		funcsym,
    period,
}SYMBOL;

/* identity types */
typedef enum OBJECT {
	variable,
	function,
}OBJECT;

/* fct code types */
typedef enum FCT {
	lit,	opr,	lod,
	sto,	cal,	ini,
	jmp,	jpc,
}FCT;

char ERR_TP[100][100];          /* error type dictionary */
char word[N_RW][LEN_ID];		/* reversed words' dict */
char mnemonic[N_FCT][5];		/* fct code dict */
enum SYMBOL ssym[256];			/* value of single-letter-type symbol */
enum SYMBOL wsym[N_RW];			/* value of reversed word symbol */
bool declbegsys[N_SYM];			/* start symbol set of declaration */
bool statbegsys[N_SYM];			/* start symbol set of statement */
bool facbegsys[N_SYM];			/* start symbol set of factor */

#ifdef __DEBUG__

char symName[N_SYM][15] =
{
    "null", "identity", "number", "plus", "minus",
    "times", "slash", "becomes", "eql", "neq",
    "lss", "leq", "gtr", "geq", "lparen",
    "rparen", "lbrace", "rbrace", "range", "semicolon",
    "ifsym", "elsesym", "forsym", "insym", "whilesym",
    "readsym", "printsym", "callsym", "varsym", "funcsym",
    "period"
};

#endif

#endif // SETTING_H_INCLUDED
