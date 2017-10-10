#include <string.h>
#include "compiler.h"

void init_setting()
{
	/* single-letter-type symbol */
    memset(ssym, 0, sizeof ssym);
    ssym['+'] = plus;
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
    ssym['%'] = mod;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['{'] = lbrace;
	ssym['}'] = rbrace;
	ssym['='] = becomes;
	ssym[';'] = semicolon;
	ssym['$'] = period;
	ssym['!'] = notsym;

	/* reversed word symbol */
	wsym[0] = callsym;
	wsym[1] = elsesym;
	wsym[2] = forsym;
	wsym[3] = funcsym;
    wsym[4] = ifsym;
	wsym[5] = insym;
	wsym[6] = printsym;
	wsym[7] = readsym;
	wsym[8] = repeatsym;
	wsym[9] = returnsym;
	wsym[10] = varsym;
    wsym[11] = whilesym;

	/* reversed words dict */
	strcpy(word[0], "call");
	strcpy(word[1], "else");
	strcpy(word[2], "for");
	strcpy(word[3], "func");
	strcpy(word[4], "if");
	strcpy(word[5], "in");
	strcpy(word[6], "print");
	strcpy(word[7], "read");
	strcpy(word[8], "repeat");
	strcpy(word[9], "return");
	strcpy(word[10], "var");
	strcpy(word[11], "while");

	/* fct code dict */
	strcpy(mnemonic[lit], "lit");
	strcpy(mnemonic[opr], "opr");
	strcpy(mnemonic[lod], "lod");
	strcpy(mnemonic[sto], "sto");
	strcpy(mnemonic[cal], "cal");
	strcpy(mnemonic[ini], "int");
	strcpy(mnemonic[jmp], "jmp");
	strcpy(mnemonic[jne], "jne");
	strcpy(mnemonic[jeq], "jeq");
	strcpy(mnemonic[jeq], "in");
	strcpy(mnemonic[jeq], "out");

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
	statbegsys[autoincre] = true;
	statbegsys[autodecre] = true;
	statbegsys[repeatsym] = true;

	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
	facbegsys[autoincre] = true;
	facbegsys[autodecre] = true;
	facbegsys[notsym] = true;
}

void init_errors()
{
    strcpy(ERR_TP[4], "declaration lacks identity");
    strcpy(ERR_TP[5], "declaration has no primary token ';'");
    strcpy(ERR_TP[6], "a wrong statement start token or function declaration");
    strcpy(ERR_TP[7], "a wrong follow token after declaration");
    strcpy(ERR_TP[8], "a wrong follow token after statement");
    strcpy(ERR_TP[9], "parsing incomplete");
    strcpy(ERR_TP[11], "a no-declaration identity");
    strcpy(ERR_TP[12], "a variable type need for lhs");
    strcpy(ERR_TP[13], "lack of become token for assignment statement");
    strcpy(ERR_TP[14], "invalid call statement without function identity");
    strcpy(ERR_TP[19], "statement ending with a wrong follow symbol");
    strcpy(ERR_TP[20], "a relation operator is lost");
    strcpy(ERR_TP[21], "invalid conversation for a function identity");
    strcpy(ERR_TP[24], "a wrong start token for factor");
    strcpy(ERR_TP[30], "too long for a number");
    strcpy(ERR_TP[31], "number is out-of-range");
    strcpy(ERR_TP[33], "invalid statement for lacking a token ')'");
    strcpy(ERR_TP[34], "invalid statement for lacking a token '('");
    strcpy(ERR_TP[36], "function needs '()' token");
    strcpy(ERR_TP[37], "statement needs start after '{' token");
    strcpy(ERR_TP[38], "statement needs ending with '}' token");
    strcpy(ERR_TP[39], "a for statement needs token 'in'");
    strcpy(ERR_TP[40], "a for statement needs a left range");
    strcpy(ERR_TP[41], "a for statement needs a right range");
    strcpy(ERR_TP[42], "a for statement needs a range symbol '...'");
    strcpy(ERR_TP[43], "no variable to use for auto-increment statement");
    strcpy(ERR_TP[44], "no variable to use for auto-decrement statement");
    strcpy(ERR_TP[45], "a while symbol is need for repeat statement");
    strcpy(ERR_TP[46], "a wrong follow symbol for logic or judgement");
    strcpy(ERR_TP[47], "a wrong follow symbol for logic and judgement");
    strcpy(ERR_TP[60], "program is too long --end");
    strcpy(ERR_TP[61], "Displacement address is too big --end");
}

void init_proc()
{
    fend_tag = 0;
	err_num = 0;
	line_num = 0;
	cc = ll = cx = 0;
	ch = ' ';

	tableswitch = 1;
	listswitch = 1;

    fresult = fopen("fresult.txt", "w");
    ferr = fopen("ferr.txt", "w");
    ftable = fopen("ftable.txt", "w");
    fcode = fopen("fcode.txt", "w");
    foutput = fopen("foutput.txt", "w");
}
