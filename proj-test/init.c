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
    ssym['&'] = bitand;
    ssym['|'] = bitor;
    ssym['~'] = bitnot;
    ssym['^'] = xor;
    ssym['!'] = notsym;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['{'] = lbrace;
	ssym['}'] = rbrace;
	ssym['='] = becomes;
	ssym[':'] = colon;
	ssym[';'] = semicolon;
	ssym['$'] = period;

	/* reversed word symbol */
	wsym[0] = boolsym;
	wsym[1] = callsym;
	wsym[2] = charsym;
	wsym[3] = elsesym;
	wsym[4] = falsesym;
	wsym[5] = forsym;
	wsym[6] = funcsym;
    wsym[7] = ifsym;
	wsym[8] = insym;
	wsym[9] = intsym;
	wsym[10] = letsym;
	wsym[11] = printsym;
	wsym[12] = readsym;
	wsym[13] = repeatsym;
	wsym[14] = returnsym;
	wsym[15] = truesym;
	wsym[16] = varsym;
    wsym[17] = whilesym;

	/* reversed words dict */
	strcpy(word[0], "bool");
	strcpy(word[1], "call");
	strcpy(word[2], "char");
	strcpy(word[3], "else");
	strcpy(word[4], "false");
	strcpy(word[5], "for");
	strcpy(word[6], "func");
	strcpy(word[7], "if");
	strcpy(word[8], "in");
	strcpy(word[9], "int");
	strcpy(word[10], "let");
	strcpy(word[11], "print");
	strcpy(word[12], "read");
	strcpy(word[13], "repeat");
	strcpy(word[14], "return");
	strcpy(word[15], "true");
	strcpy(word[16], "var");
	strcpy(word[17], "while");

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
	strcpy(mnemonic[in], "in");
	strcpy(mnemonic[out], "out");

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
	statbegsys[returnsym] = true;

	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
	facbegsys[autoincre] = true;
	facbegsys[autodecre] = true;
	facbegsys[notsym] = true;
	facbegsys[truesym] = true;
	facbegsys[falsesym] = true;
}

void init_errors()
{
    strcpy(ERR_TP[4], "declaration lacks identity");
    strcpy(ERR_TP[5], "lack primary token ';'");
    strcpy(ERR_TP[6], "a wrong statement start token or function declaration");
    strcpy(ERR_TP[7], "a wrong follow token after declaration");
    strcpy(ERR_TP[8], "a wrong follow token after statement");
    strcpy(ERR_TP[9], "parsing incomplete");
    strcpy(ERR_TP[11], "a no-declaration identity");
    strcpy(ERR_TP[12], "a variable type need for lhs");
    strcpy(ERR_TP[13], "lack of become token for assignment statement");
    strcpy(ERR_TP[14], "invalid call statement without function identity");
    strcpy(ERR_TP[15], "a declaration of constant type must initialize with a value");
    strcpy(ERR_TP[16], "a non-variable type instance cannot be auto-increment or auto-decrement");
    strcpy(ERR_TP[17], "a datatype token is needed after ':' for a data declaration");
    strcpy(ERR_TP[18], "a datatype token is needed after '->' for a function declaration to return");
    strcpy(ERR_TP[19], "statement ending with a wrong follow symbol");
    strcpy(ERR_TP[20], "a relation operator is lost");
    strcpy(ERR_TP[21], "invalid conversation for a function identity");
    strcpy(ERR_TP[24], "a wrong start token for factor");
    strcpy(ERR_TP[25], "non-variable identity cannot be read by input");
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
    strcpy(ERR_TP[48], "the function needs a return statement");
    strcpy(ERR_TP[49], "a value must return for the function");
    strcpy(ERR_TP[60], "program is too long --end");
    strcpy(ERR_TP[61], "Displacement address is too big --end");
}

void init_proc()
{
    fend_tag = 0;
	err_num = 0;
	line_num = 0;
	rtn_type = pretermit;
	cc = ll = cx = 0;
	ch = ' ';

	tableswitch = 1;
	listswitch = 1;
    #ifdef __DEBUG__
	fresult = fopen("fresult.txt", "w");
    ftable = fopen("ftable.txt", "w");
    foutput = fopen("foutput.txt", "w");
    #endif
    fcode = fopen("fcode.json", "w");
    ferr = fopen("ferr.json", "w");
    fprintf(ferr, "{\n\"errors\":[\n");
}
