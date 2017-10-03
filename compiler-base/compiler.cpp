#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "compiler.h"

using namespace std;


int main()
{
    init();
	for(int i = 0; i < 256; ++i)
		printf("%d\n", ssym[i]);
}

void init()
{
	init_setting();
	init_errors();
	init_proc();
}

/*
 *
 */
int inset(int e, bool* s)
{
	return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]||s2[i];
	}
	return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&(!s2[i]);
	}
	return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&s2[i];
	}
	return 0;
}

void error(int n)
{
    // fprintf(ferr, "# line %d: %s\n", line_num, ERR_TP[n]);

    err_num++;
    if(err_num > MAX_ERR)
    {
        exit(1);
    }
}

void getch()
{
	if (cc == ll)
	{
		if (feof(fin))
		{
			printf("Promgram is incomplete.");
			exit(1);
		}
		ll = cc = 0;
		line_num++;
		printf("%d ", cx);
		//fprintf(foutput, "%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}

			printf("%c", ch);
			//fprintf(foutput, "%c", ch);
			line[ll++] = ch;
		}
	}
	ch = line[cc];
	cc++;
}

void getsym()
{
	int i, j, k;

	while (ch == ' ' || ch == 10 || ch == 9)
		getch();
	if (isalpha(ch))    // a reversed word or identity begins with alpha
	{
		k = 0;
		do {
			if (k < LEN_ID)
				a[k++] = ch;
			getch();
		} while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		i = 0;
		j = N_RW-1;
		do {    // search for reversed words
            k = (i + j) / 2;
            if (strcmp(id, word[k]) <= 0)
                j = k - 1;

            if (strcmp(id, word[k]) >= 0)
                i = k+1;

		} while (i <= j);
		if (i-1 > j)    // current symbol is reversed word
            sym = wsym[k];

        else        // current symbol is identity
            sym = ident;
	}
	else
    {
        if (isdigit(ch))    // current symbol is number
        {
            k = 0;
            num = 0;
            sym = number;
            do {
                num = 10 * num + ch - '0';
                k++;
                getch();
            } while (isdigit(ch));  // gain the whole number
            k--;
            if (k > LEN_NUM)
                error(30);
        }
        else
        {
            if (ch == '.')	// test symbol '...' for 'for _ in _ ... _' structure
            {
                getch();
                if(ch == '.')
                {
                    getch();
                    if(ch == '.')
                    {
                        sym = range;
                        getch();
                    }
                    else sym = nul;
                }
                else sym = nul;
            }
            else if (ch == '=')	// test symbol '==' or '='
            {
                getch();
                if (ch == '=')
                {
                    sym = eql;
                    getch();
                }
                else
                    sym = becomes;
            }
            else if (ch == '!') // test symbol '!='
            {
                getch();
                if (ch == '=')
                {
                    sym = neq;
                    getch();
                }
                else
                    sym = nul;
            }
            else if (ch == '>') // test symbol '>=' or '>'
            {
                getch();
                if (ch == '=')
                {
                    sym = geq;
                    getch();
                }
                else
                    sym = gtr;
            }
            else if (ch == '<')	// test symbol '<=' or '<'
            {
                getch();
                if (ch == '=')
                {
                    sym = leq;
                    getch();
                }
                else
                    sym = lss;
            }
            else	// other single-char-type symbols
            {
                sym = ssym[ch];
                getch();
            }
        }
	}
}

/*
 * generate fct codes
 *
 * x: Instruction.f
 * y: Instruction.l
 * z: Instruction.a
 */
void gen(enum FCT x, int y, int z)
{
    if (cx >= MAX_CX)
    {
        error(60);
    }
    if (z >= BOUND_ADR)
    {
        error(61);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx++;
}

/*
 * test current symbol's legality
 *
 * s1:		need words
 * s2:		words for recovering stage
 * n:		error type
 */
 void test(bool* s1, bool* s2, int n)
{
 	if (!inset(sym, s1))
 	{
 		error(n);
 		/* skip symbols until get need words or recovering words */
 		while ((!inset(sym, s1)) && (!inset(sym, s2)))
 		{
 			getsym();
 		}
 	}
 }

/*
 * Main body for compiler and block processing
 *
 * lev:		0 means main block, 1 means function body
 * tx:      tail pointer for symbol list
 * fsys:    follow set of current block
 */
void block(int lev, int tx, bool* fsys)
{
    int dx;             /* record of relative address of data */
    int tx0;            /* record of original tx */
    int cx0;            /* record of original cx */
    bool nxtlev[N_SYM]; /* passing symbol set for next level */

    dx = 3;
    tx0 = tx;
    table[tx].adr = cx;
    gen(jmp, 0, 0);     /* generate jmp code */

    /* declaration list parsing */
    do {
        /* variable declaration parsing */
        while (sym == varsym)
        {
            getsym();
            declaration(variable, &tx, lev, &dx);
            getsym();
            if (sym == semicolon)
                getsym();
            else error(5);
        }

        /* function declaration parsing */
        while (sym == funcsym)
        {
            getsym();
            declaration(function, &tx, lev, &dx);

            if (sym == lparen) getsym();	// processing ()
            else error(34);
            if (sym == rparen) getsym();
            else error(34);
            if (sym == lbrace) getsym();	// processing {
            else error(35);

            /* function declaration nesting is not allowed */
            if(lev == 0){
	            memcpy(nxtlev, fsys, sizeof nxtlev);
    	        nxtlev[semicolon] = true;
    	        block(1, tx, nxtlev);
			}

			if (sym == rbrace)   // processing }
			{
				getsym();
				memcpy(nxtlev, statbegsys, sizeof nxtlev);
				nxtlev[funcsym] = true;
				test(nxtlev, fsys, 6);  // after a function should be a statement or another function
			}
			else error(36);
        }

        test(statbegsys, declbegsys, 7);    // test if there is a correct statement start
    } while (inset(sym, declbegsys));   // processing until no declaration symbols

    /* update symbol table & fct codes */
    code[table[tx0].adr].a = cx;
    table[tx0].adr = cx;
    table[tx0].size = dx;
    cx0 = cx;
    gen(ini, 0, dx);

    if (tableswitch)
    {
        for(int i = 1; i <= tx; ++i)
        {
            switch (table[i].kind)
            {
                case variable:
                    fprintf(ftable, "    %d var   %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d\n", table[i].level, table[i].adr);
                    break;
                case function:
                    fprintf(ftable,"    %d func  %s ", i, table[i].name);
                    fprintf(ftable,"lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
                    break;
            }
        }
        fprintf(ftable, "\n");
    }

    /* statement list parsing */
    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[semicolon] = true;
    statement(nxtlev, &tx, lev);
    gen(opr, 0, 0);
    memset(nxtlev, 0, sizeof nxtlev);
    test(fsys, nxtlev, 8);
    listcode(cx0);
}

/* 
 * add new to symbol table
 *
 * k:		type of symbol
 * ptx:		tail pointer of symbol table
 * lev:		symbol is in main block or function body
 * pdx:		relative address for current variable
 */
void enter(enum OBJECT k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id);
	table[(*ptx)].kind = k;
	switch(k)
	{
		case variable:
			table[(*ptx)].level = lev;
			table[(*ptx)].adr = (*pdx);
			(*pdx)++;
			break;
		case function:
			table[(*ptx)].level = lev;
			break;
	}
}

/*
 * searching for the identity's position in symbol table
 * 0 represents not exists
 *
 * id:      name of identity
 * tx:      current tail pointer of symbol table
 */
int position(char* id, int tx)
{
    int i = tx;
    strcpy(table[0].name, id);
    while (strcmp(table[i].name, id) != 0)
        --i;
    return i;
}

/*
 * declaration processing
 *
 * tp:      type of declaration processing
 * ptx:		tail pointer of symbol table
 * lev:		symbol is in main block or function body
 * pdx:		relative address for current variable
 */
void declaration(enum OBJECT tp, int *ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		enter(tp, ptx, lev, pdx);
		getsym();
	}
	else
		error(4);
}

/*
 * statement processing
 */
void statement(bool* fsys, int* ptx, int lev)
{
    int p, cx1, cx2;
    bool nxtlev[N_SYM];

    if (sym == ident)
    {
        p = position(id, *ptx);
        if (p == 0) error(11);
        else
        {
            if (table[p].kind != variable)
            {
                
            }
        }
    }
}

/*
 * output code list
 */
void listcode(int cx0)
{
    if(listswitch)
    {
        puts("");
        for(int i = cx0; i < cx; ++i)
        {
            printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/*
 * output all codes
 */
void listall()
{
    if(listswitch)
    {
        for (int i = 0; i < cx; ++i)
        {
            printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
            fprintf(fcode,"%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}