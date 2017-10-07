#include <string.h>
#include <ctype.h>
#include "compiler.h"

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
    fprintf(ferr, "# line %d: %s\n", line_num, ERR_TP[n]);

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
		if (feof(fin)){
		    ch = '$';
		    return;
		}

		ll = cc = 0;
		line_num++;

		printf("%d ", cx);
		fprintf(foutput, "%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}
            printf("%c", ch);
			fprintf(foutput, "%c", ch);
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
                i = k + 1;

		} while (i <= j);
		if (i-1 > j)    // current symbol is reversed word
            sym = wsym[k];

        else        // current symbol is identity
            sym = ident;
	}
	else if (isdigit(ch))    // current symbol is number
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
    else if (ch == '.')	// test symbol '...' for 'for _ in _ ... _' structure
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
        if (sym != period)
            getch();
    }
}


/*
 * generate fct codes
 *
 * x: Instruction.f
 * y: Instruction.l
 * z: Instruction.a
 */
void gen(FCT x, int y, int z)
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
void problem(int lev, int tx, bool* fsys)
{
    int dx;             /* record of relative address of data */
    int tx0;            /* record of original tx */
    int cx0;            /* record of original cx */
    bool nxtlev[N_SYM]; /* passing symbol set for next level */
    bool declbegsys_t[N_SYM];

    dx = 3;
    tx0 = tx;
    table[tx].adr = cx;
    gen(jmp, 0, 0);     /* generate jmp code */

    memcpy(declbegsys_t, declbegsys, sizeof declbegsys);
    if (lev == 1)
        declbegsys_t[funcsym] = false;

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
        /* function declaration nesting is not allowed */
        while (lev == 0 && sym == funcsym)
        {
            getsym();
            declaration(function, &tx, lev, &dx);

            if (sym == lparen) getsym();	// processing ()
            else error(36);
            if (sym == rparen) getsym();
            else error(36);
            if (sym == lbrace) getsym();	// processing {
            else error(37);

            memcpy(nxtlev, fsys, sizeof nxtlev);
            nxtlev[semicolon] = true;
            problem(1, tx, nxtlev);

			if (sym == rbrace)   // processing }
			{
				getsym();
				memcpy(nxtlev, statbegsys, sizeof nxtlev);
				nxtlev[funcsym] = true;
				test(nxtlev, fsys, 6);  // after a function should be a statement or another function
			}
			else error(37);
        }

        test(statbegsys, declbegsys_t, 7);    // test if there is a correct statement start
    } while (inset(sym, declbegsys_t));   // processing until no declaration symbols

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
void enter(OBJECT k, int* ptx, int lev, int* pdx)
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
void declaration(OBJECT tp, int *ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		enter(tp, ptx, lev, pdx);
		getsym();
	}
	else
		error(4);
}


void forstatrange(bool* fsys, int* ptx, int lev)
{
	int i;
	switch (sym)
	{
		case ident:
	    	i = position(id, *ptx);
			if (i == 0) error(11);  // a no-declaration identity
			else
			{
			    switch (table[i].kind)
			    {
                    case variable:
				        gen(lod, lev-table[i].level, table[i].adr);
		                break;
			        case function:
				        error(21);  // cannot be a function
				        i = 0;
				        break;
				    }
				}
				getsym();
	    		break;
	   	case number:
	    	if (num > BOUND_ADR)
			{
				error(31);  // number out of range
				num = 0;
			}
			gen(lit, 0, num);
			getsym();
	    	break;
	    default:
	    	error(40);	// lack a left range
	    	break;
	}
}

/*
 * statement processing
 */
void statement(bool* fsys, int* ptx, int lev)
{
    int i, cx1, cx2;
    bool nxtlev[N_SYM];

    /* assignment statement parsing */
    if (sym == ident)
    {
        i = position(id, *ptx);
        if (i == 0) error(11);
        else
        {
            if (table[i].kind != variable)
                error(12);
            else
            {
                getsym();
                if(sym == becomes)
                    getsym();
                else error(13); // no becomes symbol
                /* expression parsing */
                memcpy(nxtlev, fsys, sizeof nxtlev);
                expression(nxtlev, ptx, lev);
                if (i != 0)
                    gen(sto, lev-table[i].level, table[i].adr);
            }
        }
    }
    /* read statement parsing */
    else if (sym == readsym)
    {
        getsym();
        if(sym != lparen)   // lack of '('
            error(34);
        else
        {
            getsym();
            if (sym == ident)
                i = position(id, *ptx);
            else i = 0;

            if (i == 0) error(35);  // identity in read() still not declared
            else
            {
                gen(opr, 0, 16);    // generate input instruction
                gen(sto, lev-table[i].level, table[i].adr); // send stack top into variable
            }
            getsym();
        }
        if (sym != rparen)
        {
            error(33);  // lacking token ')'
            while (!inset(sym, fsys))   // recover with last level follow set
                getsym();
        }
        else getsym();
    }
    /* write statement parsing */
    else if (sym == printsym)
    {
        getsym();
        if(sym != lparen)   // lack of '('
            error(34);
        else
        {
            getsym();
            if (sym == ident)
                i = position(id, *ptx);
            else i = 0;

            if (i == 0) error(35);  // identity in print() still not declared
            else
            {
                gen(opr, 0, 14);    // generate output instruction
                gen(opr, 0, 15);    // generate line instruction
            }
            getsym();
        }
        if (sym != rparen)
        {
            error(33);  // lacking token ')'
            while (!inset(sym, fsys))   // recover with last level follow set
                getsym();
        }
        else getsym();
    }
    /* call statement parsing */
    else if (sym == callsym)
    {
        getsym();
        if (sym != ident)
            error(14);  // lacks call identity
        else
        {
            i = position(id, *ptx);
            if (i == 0)
                error(11);  // function identity without declaration
            else
            {
                if (table[i].kind == function)
                    gen(cal, lev-table[i].level, table[i].adr);
                else error(15); // identity isn't a function
            }
            getsym();

            // processing '()'
            if (sym == lparen) getsym();
            else error(36);
            if (sym == rparen) getsym();
            else error(36);
        }
    }
    /* if statement parsing */
    else if (sym == ifsym)
    {
        getsym();
        memcpy(nxtlev, fsys, sizeof nxtlev);
        nxtlev[lbrace] = true;
        condition(nxtlev, ptx, lev);

        if (sym == lbrace) getsym();
        else error(37); // a token '{' after condition

        /* conditional jump to false end */
        cx1 = cx;
        gen(jpc, 0, 0);

        memcpy(nxtlev, fsys, sizeof nxtlev);
        nxtlev[rbrace] = true;
        nxtlev[elsesym] = true;
        statement(nxtlev, ptx, lev);

        /* unconditional jump to true end */
        cx2 = cx;
        gen(jmp, 0, 0);
        code[cx1].a = cx;   // backfill the conditional jump's address

        if (sym == rbrace) getsym();
        else error(38); // a token '}' after statement

        if (sym == elsesym)
        {
            getsym();
            if (sym == lbrace) getsym();
            else error(37); // statement in else block needs '{'

            memcpy(nxtlev, fsys, sizeof nxtlev);
            nxtlev[rbrace] = true;
            statement(nxtlev, ptx, lev);

            if (sym == rbrace) getsym();
            else error(38); // statement in else block needs '}'
        }
        code[cx2].a = cx;
    }
    /* while statement parsing */
    else if (sym == whilesym)
    {
        cx1 = cx;
        getsym();
        memcpy(nxtlev, fsys, sizeof nxtlev);
        nxtlev[lbrace] = true;
        condition(nxtlev, ptx, lev);
        cx2 = cx;
        gen(jpc, 0, 0);

        if (sym == lbrace) getsym();
        else error(37); // lack '{'

        memcpy(nxtlev, fsys, sizeof nxtlev);
        nxtlev[rbrace] = true;
        statement(nxtlev, ptx, lev);

        if (sym == rbrace) getsym();
        else error(38); // lack '}'

        gen(jmp, 0, cx1);
        code[cx2].a = cx;
    }
    /* for statement parsing */
    else if (sym == forsym)
    {
    	getsym();
    	if (sym == ident)
    	{
    		i = position(id, *ptx);
    		if (i == 0) error(11);
    		else
    		{
    			if (table[i].kind != variable)
    			{
    				error(12);
    				i = 0;
    			}
    			else
    			{
	    			getsym();
	    			if (sym != insym) error(39);	// lack 'in'
	    			else getsym();

	    			forstatrange(fsys, ptx, lev);
	    			gen(sto, lev-table[i].level, table[i].adr);

	    			if (sym != range) error(42);	// lack '...'
	    			else getsym();

	    			cx1 = cx;
	    			gen(lod, lev-table[i].level, table[i].adr);
	    			forstatrange(fsys, ptx, lev);

	    			/* condition judgement of 'for' range */
	    			gen(opr, 0, 13);
	    			/* out-of-range conditional jump */
	    			cx2 = cx;
	    			gen(jpc, 0, 0);

	    			if (sym == lbrace)
	    				getsym();
	    			else error(37);

	    			memcpy(nxtlev, fsys, sizeof nxtlev);
	    			nxtlev[rbrace] = true;
	    			statement(nxtlev, ptx, lev);

	    			if (sym == rbrace)
	    				getsym();
	    			else error(38);

	    			/* load range variable */
	    			gen(lod, lev-table[i].level, table[i].adr);
	    			gen(lit, 0, 1);	// step of range
	    			gen(opr, 0, 2);	// add step to variable
	    			/* save range variable */
	    			gen(sto, lev-table[i].level, table[i].adr);
	    			gen(jmp, 0, cx1);
	    			code[cx2].a = cx;
	    		}
    		}
    	}
    }

    memset(nxtlev, 0, sizeof nxtlev);
    test(fsys, nxtlev, 19); // follow a wrong symbol
}

/*
 * condition processing
 */
void condition(bool* fsys, int* ptx, int lev)
{
    SYMBOL relop;
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[eql] = true;
    nxtlev[neq] = true;
    nxtlev[lss] = true;
    nxtlev[leq] = true;
    nxtlev[gtr] = true;
    nxtlev[geq] = true;
    expression(nxtlev, ptx, lev);
    if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq)
        error(20);
    else
    {
        relop = sym;
        getsym();
        expression(fsys, ptx, lev);
        switch(relop)
        {
            case eql:
                gen(opr, 0, 8);
                break;
            case neq:
                gen(opr, 0, 9);
                break;
            case lss:
                gen(opr, 0, 10);
                break;
            case geq:
                gen(opr, 0, 11);
                break;
            case gtr:
                gen(opr, 0, 12);
                break;
            case leq:
                gen(opr, 0, 13);
                break;
            default:
                break;
        }
    }
}

/*
 * expression processing
 */
void expression(bool* fsys, int* ptx, int lev)
{
    SYMBOL addop = nul;
    bool nxtlev[N_SYM];

    if (sym == plus || sym == minus) // expression start with '+'|'-'
    {
        addop = sym;
        getsym();
    }

    /* term parsing */
    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[plus] = true;
    nxtlev[minus] = true;
    term(nxtlev, ptx, lev);

    if (addop == minus)  // generate negative command if expr start with '-'
        gen(opr, 0, 1);

    while (sym == plus || sym == minus)
    {
        addop = sym;
        getsym();
        /* term parsing */
        term(nxtlev, ptx, lev);

        if (addop == plus)      // generate add instruction
            gen(opr, 0, 2);
        else gen(opr, 0, 3);    // generate minus instruction
    }
}

/*
 * term processing
 */
void term(bool* fsys, int* ptx, int lev)
{
    SYMBOL mulop;
    bool nxtlev[N_SYM];

    /* term parsing */
    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[times] = true;
    nxtlev[slash] = true;
    factor(nxtlev, ptx, lev);

    while (sym == times || sym == slash)
    {
        mulop = sym;
        getsym();
        /* factor parsing */
        factor(nxtlev, ptx, lev);

        if(mulop == times)      // generate times instruction
            gen(opr, 0, 4);
        else gen(opr, 0, 5);    // generate slash instruction
    }
}

/*
 * factor processing
 */
void factor(bool* fsys, int* ptx, int lev)
{
    int i;
    bool nxtlev[N_SYM];

    test(facbegsys, fsys, 24);
    while (inset(sym, facbegsys))
    {
        /* the factor is a identity */
        if (sym == ident)
        {
            i = position(id, *ptx);
            if (i == 0) error(11);  // a no-declaration identity
            else
            {
                switch (table[i].kind)
                {
                    case variable:
                        gen(lod, lev-table[i].level, table[i].adr);
                        break;
                    case function:
                        error(21);  // cannot be a function
                        break;
                }
            }
            getsym();
        }
        /* the factor is a number */
        else if (sym == number)
        {
            if (num > BOUND_ADR)
            {
                error(31);  // number out of range
                num = 0;
            }
            gen(lit, 0, num);
            getsym();
        }
        /* the factor is a expression */
        else if (sym == lparen)
        {
            getsym();
            memcpy(nxtlev, fsys, sizeof nxtlev);
            nxtlev[rparen] = true;
            expression(nxtlev, ptx, lev);
            if (sym == rparen)
                getsym();
            else error(22); // lack ')'
        }
        memset(nxtlev, 0, sizeof nxtlev);
        nxtlev[lparen] = true;
        test(fsys, nxtlev, 23);
    }
}

/*
 * interpret the intermediate code
 */
void interpret()
{
    int p = 0;      /* pointer of code */
    int b = 1;      /* base address of code */
    int t = 0;      /* pointer of stack top */
    Instruction i;  /* store current code */
    int s[SIZE_STACK];  /* stack */

    s[0] = 0;
    s[1] = 0;
    s[2] = 0;
    s[3] = 0;
    do {
        i = code[p];    // load current code
        p = p + 1;
        switch (i.f)
        {
            case lit:   /* load a onto stack top */
                t = t + 1;
                s[t] = i.a;
                break;
            case opr:   /* math & logic operation */
                switch (i.a)
                {
                    case 0: /* return after function calling */
                        t = b - 1;
                        p = s[t + 3];
                        b = s[t + 2];
                        break;
                    case 1: /* inverse of stack top */
                        s[t] = -s[t];
                        break;
                    case 2: /* push stack top and second sum into stack */
                        t = t - 1;
                        s[t] = s[t] + s[t + 1];
                        break;
                    case 3: /* stack second minus top */
                        t = t - 1;
                        s[t] = s[t] - s[t + 1];
                        break;
                    case 4: /* stack second times top */
                        t = t - 1;
                        s[t] = s[t] * s[t + 1];
                        break;
                    case 5: /* stack second slash top */
                        t = t - 1;
                        s[t] = s[t] / s[t + 1];
                        break;
                    case 6: /* odd judgement of stack top */
                        s[t] = s[t] & 1;
                        break;
                    case 8: /* stack top and second equality judgement */
                        t = t - 1;
                        s[t] = (s[t] == s[t + 1]);
                        break;
                    case 9: /* stack top and second not-equality judgement */
                        t = t - 1;
                        s[t] = (s[t] != s[t + 1]);
                        break;
                    case 10:/* stack second less than top judgement */
                        t = t - 1;
                        s[t] = (s[t] < s[t + 1]);
                        break;
                    case 11:/* stack second larger than or equal to top judgement */
                        t = t - 1;
                        s[t] = (s[t] >= s[t + 1]);
                        break;
                    case 12:/* stack second larger than top judgement */
                        t = t - 1;
                        s[t] = (s[t] > s[t + 1]);
                        break;
                    case 13:/* stack second less than or equal to top judgement */
                        t = t - 1;
                        s[t] = (s[t] <= s[t + 1]);
                        break;
                    case 14:/* output stack top */
                        fprintf(fresult, "%d", s[t]);
                        t = t - 1;
                        break;
                    case 15:/* output line break */
                        fprintf(fresult, "\n");
                        break;
                    case 16:/* read an input into stack top */
                        t = t + 1;
                        printf("Enter the variable:\n");
                        scanf("%d", &(s[t]));
                        fprintf(fresult, "%d\n", s[t]);
                        break;
                }
                break;
            case lod:   /* push the value in memory into stack with current function base and offset address */
                t = t + 1;
                s[t] = s[base(i.l, s, b) + i.a];
                break;
            case sto:   /* save the stack top value into memory with current function base and offset address */
                s[base(i.l, s, b) + i.a] = s[t];
                t = t - 1;
                break;
            case cal:   /* call function */
                s[t + 1] = base(i.l, s, b);
                s[t + 2] = b;
                s[t + 3] = p;
                b = t + 1;
                p = i.a;
                break;
            case ini:   /* initialize a size data space for function */
                t = t + i.a;
                break;
            case jmp:   /* unconditional jump */
                p = i.a;
                break;
            case jpc:   /* conditional jump */
                if (s[t] == 0)
                    p = i.a;
                t = t - 1;
                break;
        }
    } while (p != 0);
}

/*
 * find base address for function with difference of level
 */
int base(int l, int* s, int b)
{
    int b1;
    b1 = b;
    while (l > 0)
    {
        b1 = s[b1];
        l--;
    }
    return b1;
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
