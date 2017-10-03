#ifndef SETTING_H_INCLUDED
#define SETTING_H_INCLUDED

#include <cstring>

#define N_SYM 30		/* symbol number */
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
enum SYMBOL {
    nul,		ident,		number, 	plus,		minus,
    times,		slash,		becomes,	eql,		neq,
    lss,		leq,		gtr,		geq,		lparen,
    rparen,		lbrace,		rbrace,		range,		semicolon,
    ifsym,		elsesym,	forsym,		insym,		whilesym,
    readsym,	printsym,	callsym,	varsym,		funcsym,
};

/* identity types */
enum OBJECT {
	variable,
	function,
};

/* fct code types */
enum FCT {
	lit,	opr,	lod,
	sto,	cal,	ini,
	jmp,	jpc,
};

char word[N_RW][LEN_ID];		/* reversed words' dict */
char mnemonic[N_FCT][5];		/* fct code dict */
enum SYMBOL ssym[256];			/* value of single-letter-type symbol */
enum SYMBOL wsym[N_RW];			/* value of reversed word symbol */
bool declbegsys[N_SYM];			/* start symbol set of declaration */
bool statbegsys[N_SYM];			/* start symbol set of statement */
bool facbegsys[N_SYM];			/* start symbol set of factor */


void init_setting()
{
	/* single-letter-type symbol */
    memset(ssym, 0, sizeof ssym);
    ssym['+'] = plus;
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['{'] = lbrace;
	ssym['}'] = rbrace;
	ssym['='] = becomes;
	ssym[';'] = semicolon;

	/* reversed word symbol */
	wsym[0] = callsym;
	wsym[1] = elsesym;
	wsym[2] = forsym;
	wsym[3] = funcsym;
    wsym[4] = ifsym;
	wsym[5] = insym;
	wsym[6] = printsym;
	wsym[7] = readsym;
	wsym[8] = varsym;
    wsym[9] = whilesym;

	/* reversed words dict */
	strcpy(word[0], "call");
	strcpy(word[1], "else");
	strcpy(word[2], "for");
	strcpy(word[3], "func");
	strcpy(word[4], "if");
	strcpy(word[5], "in");
	strcpy(word[6], "print");
	strcpy(word[7], "read");
	strcpy(word[8], "var");
	strcpy(word[9], "while");

	/* fct code dict */
	strcpy(mnemonic[lit], "lit");
	strcpy(mnemonic[opr], "opr");
	strcpy(mnemonic[lod], "lod");
	strcpy(mnemonic[sto], "sto");
	strcpy(mnemonic[cal], "cal");
	strcpy(mnemonic[ini], "int");
	strcpy(mnemonic[jmp], "jmp");
	strcpy(mnemonic[jpc], "jpc");

	/* start sets setting */
	declbegsys[varsym] = true;
	declbegsys[funcsym] = true;

	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
	statbegsys[readsym] = true;
	statbegsys[printsym] = true;
	statbegsys[ident] = true;
	statbegsys[forsym] = true;
	statbegsys[callsym] = true;

	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
}

#endif // SETTING_H_INCLUDED
