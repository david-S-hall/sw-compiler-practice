#include <string.h>
#include <ctype.h>
#include "compiler.h"

#define __DEBUG__

#ifdef __DEBUG__

char symNames[N_SYM][15] =
{
    "null",     "identity", "number",   "plus",     "minus",
    "times",    "slash",    "mod",      "plusbe",   "minusbe",
    "timesbe",  "slashbe",  "modbe",    "becomes",  "eql",
    "neq",      "lss",      "leq",      "gtr",      "geq",
    "lparen",   "rparen",   "lbrace",   "rbrace",   "range",
    "semicolon","ifsym",    "elsesym",  "forsym",   "insym",
    "whilesym", "readsym", "printsym", "callsym", "varsym", "funcsym",
    "period", "autoincre", "autodecre", "returnsym", "repeatsym",
     "andsym", "orsym", "notsym"
};

#endif

void init()
{
	init_setting();
	init_errors();
	init_proc();
}

void parsing()
{
    bool nxtlev[N_SYM];

    init();
    getsym();

    addset(nxtlev, declbegsys, statbegsys, N_SYM);
    nxtlev[period] = true;
    problem(0, 0, nxtlev);

    if (sym != period)
        error(9);
}

void processing()
{
    fprintf(ferr, "=== Build finished: ");
    if (err_num)
    {
        fprintf(ferr, "%d error(s)", err_num);
        if (err_num > MAX_ERR)
        {
            fprintf(ferr, ", more errors are collapsed");
        }
        fprintf(ferr, " ===\n");
    }
    else
    {
        fprintf(ferr, "Parsing success ===\n");

        listall();
        fclose(fcode);

        interpret();
        fclose(fresult);
    }

    fclose(fin);
    fclose(ferr);
    fclose(ftable);
    fclose(foutput);
}

void error(int n)
{
    err_num++;
    if(err_num > MAX_ERR)
    {
        processing();
        return;
    }

    fprintf(ferr, "# line %d, type %d: %s\n", line_num, n, ERR_TP[n]);
}

void getch()
{
    if (fend_tag) return;
	if (cc == ll)
	{
		if (feof(fin)){
		    ch = '$';
		    fend_tag = 1;
		    return;
		}

		ll = cc = 0;
		line_num++;

		//printf("%d ", cx);
		fprintf(foutput, "%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}
            //printf("%c", ch);
			fprintf(foutput, "%c", ch);
			line[ll++] = ch;
		}
	}
	ch = line[cc];
	cc++;
}

void getsym()
{
    if (sym == period)
        return;

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
            else if (ch == '<')
            {
                sym = halfrange;
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
            sym = notsym;
    }
    else if (ch == '>') // test symbol '>=' or '>'
    {
        getch();
        if (ch == '=')
        {
            sym = geq;
            getch();
        }
        else if (ch == '>')
        {
            getch();
            if (ch == '=')
            {
                sym = shrbe;
                getch();
            }
            else
                sym = shr;
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
        else if (ch == '<')
        {
            getch();
            if (ch == '=')
            {
                sym = shlbe;
                getch();
            }
            else
                sym = shl;
        }
        else
            sym = lss;
    }
    else if (ch == '+')  // test symbol '++' or '+'
    {
        getch();
        if (ch == '+')
        {
            sym = autoincre;
            getch();
        }
        else if(ch == '=')
        {
            sym = plusbe;
            getch();
        }
        else
            sym = plus;
    }
    else if (ch == '-') // test symbol '--' or '-'
    {
        getch();
        if (ch == '-')
        {
            sym = autodecre;
            getch();
        }
        else if (ch == '=')
        {
            sym = minusbe;
            getch();
        }
        else
            sym = minus;
    }
    else if (ch == '*')
    {
        getch();
        if (ch == '=')
        {
            sym = timesbe;
            getch();
        }
        else
            sym = times;
    }
    else if (ch == '/')
    {
        getch();
        if (ch == '*')  // block comment parsing
        {
            char a = 0, b = 0;
            getch();
            a = ch;
            while(1)
            {
                b = a;
                getch();
                a = ch;
                /* normal break for comment ending */
                if (b == '*' && a == '/') break;
                /* special judge for EOF break */
                if (fend_tag) break;
            }
            getch();
            getsym();
            return;
        }
        else if (ch == '/') // line comment parsing
        {
            getch();
            while (1)
            {
                /* normal break for comment ending */
                if (ch == 10) break;
                /* special judge for EOF break */
                if (fend_tag) break;
                getch();
            }
            getch();
            getsym();
            return;
        }
        else if (ch == '=')
        {
            sym = slashbe;
            getch();
        }
        else
            sym = slash;
    }
    else if (ch == '%')
    {
        getch();
        if (ch == '=')
        {
            sym = modbe;
            getch();
        }
        else
            sym = mod;
    }
    else if (ch == '&')
    {
        getch();
        if (ch == '&')
        {
            sym = andsym;
            getch();
        }
        else if (ch == '=')
        {
            sym = andbe;
            getch();
        }
        else
            sym = bitand;
    }
    else if (ch == '|')
    {
        getch();
        if (ch == '|')
        {
            sym = orsym;
            getch();
        }
        else if (ch == '=')
        {
            sym = orbe;
            getch();
        }
        else
            sym = bitor;
    }
    else if (ch == '^')
    {
        getch();
        if (ch == '=')
        {
            sym = xorbe;
            getch();
        }
        else
            sym = xor;
    }
    else	// other single-char-type symbols
    {
        sym = ssym[ch];
        if (sym != period)
            getch();
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
    if (lev == 0)
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
            nxtlev[rbrace] = true;
            problem(1, tx, nxtlev);

			if (sym == rbrace)   // processing }
			{
				getsym();
				memcpy(nxtlev, statbegsys, sizeof nxtlev);
				nxtlev[funcsym] = true;
				test(nxtlev, fsys, 6);  // after a function should be a statement or another function
			}
			else
                error(38);  // lack '}'
        }

        test(fsys, declbegsys_t, 7);    // test if there is a correct follow of declaration
    } while (inset(sym, declbegsys_t));   // processing until no declaration symbols

    /* update symbol table & fct codes */
    if (lev == 0)
        code[table[tx0].adr].a = cx;
    table[tx0].adr = cx;
    table[tx0].size = dx;
    #ifdef __DEBUG__
    cx0 = cx;
    #endif
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
    #ifdef __DEBUG__
    listcode(cx0);
    #endif
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

    while (inset(sym, statbegsys)){
        /* assignment statement parsing start with identity */
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
                    if (sym == becomes || sym == plusbe || sym == minusbe ||
                        sym == timesbe || sym == slashbe || sym == modbe ||
                        sym == andbe || sym == orbe || sym == xorbe || sym == shlbe || sym == shrbe)
                    {
                        int assop = sym;
                        getsym();

                        if (assop != becomes)
                            gen(lod, lev-table[i].level, table[i].adr);
                        /* expression parsing */
                        memcpy(nxtlev, fsys, sizeof nxtlev);
                        expression(nxtlev, ptx, lev);

                        switch (assop)
                        {
                            case becomes: break;
                            case plusbe:
                                gen(opr, 0, 2);
                                break;
                            case minusbe:
                                gen(opr, 0, 3);
                                break;
                            case timesbe:
                                gen(opr, 0, 4);
                                break;
                            case slashbe:
                                gen(opr, 0, 5);
                                break;
                            case modbe:
                                gen(opr, 1, 5);
                                break;
                            case andbe:
                                gen(opr, 0, 10);
                                break;
                            case orbe:
                                gen(opr, 0, 11);
                                break;
                            case xorbe:
                                gen(opr, 0, 13);
                                break;
                            case shlbe:
                                gen(opr, 0, 16);
                                break;
                            case shrbe:
                                gen(opr, 1, 16);
                                break;
                            default: break;
                        }

                        gen(sto, lev-table[i].level, table[i].adr);
                    }
                    else if (sym == autoincre || sym == autodecre)
                    {
                        int op = (sym == autoincre ? 2 : 3);
                        getsym();
                        /* load variable */
    	    			gen(lod, lev-table[i].level, table[i].adr);
    	    			gen(lit, 0, 1);	// step of autoincre | autodecre
    	    			gen(opr, 0, op);// add step to variable
    	    			/* save variable */
    	    			gen(sto, lev-table[i].level, table[i].adr);
                    }
                    else error(13); // lack of '='
                }
            }
        }
        /* pre-autoincrement | pre-autodecrement assignment statement parsing */
        else if (sym == autoincre || sym == autodecre)
        {
            int op = (sym == autoincre ? 2 : 3);
            //printf("op=%d\n", op);
            getsym();
            if (sym != ident)
                op == 2 ? error(43) : error(44);
            else
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

                        /* load variable */
    	    			gen(lod, lev-table[i].level, table[i].adr);
    	    			gen(lit, 0, 1);	// step of autoincre | autodecre
    	    			gen(opr, 0, op);// add step to variable
    	    			/* save variable */
    	    			gen(sto, lev-table[i].level, table[i].adr);
                    }
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
                    gen(in, 0, 0);    // generate input instruction
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
                memcpy(nxtlev, fsys, sizeof nxtlev);
                nxtlev[rparen] = true;
                expression(nxtlev, ptx, lev);
                gen(out, 0, 0);    // generate output instruction
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
            expression(nxtlev, ptx, lev);

            if (sym == lbrace) getsym();
            else error(37); // a token '{' after condition

            /* conditional jump to false end */
            cx1 = cx;
            gen(jne, 0, 0);

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
            expression(nxtlev, ptx, lev);
            cx2 = cx;
            gen(jne, 0, 0);

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

    	    			int rgeop = sym;
    	    			if (sym == range || sym == halfrange) getsym();
    	    			else error(42);	// lack '...' or '..<'

    	    			cx1 = cx;
    	    			gen(lod, lev-table[i].level, table[i].adr);
    	    			forstatrange(fsys, ptx, lev);

    	    			/* condition judgement of 'for' range */
    	    			switch(rgeop)
    	    			{
                            case range:
                                gen(opr, 1, 8);
                                break;
                            case halfrange:
                                gen(opr, 0, 8);
                                break;
                            default: break;
    	    			}
    	    			/* out-of-range conditional jump */
    	    			cx2 = cx;
    	    			gen(jne, 0, 0);

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
        /* repeat statement parsing */
        else if (sym == repeatsym)
        {
            getsym();
            if (sym == lbrace) getsym();
            else error(37); // lack '}'

            cx1 = cx;
            memcpy(nxtlev, fsys, sizeof nxtlev);
            nxtlev[rbrace] = true;
            statement(nxtlev, ptx, lev);

            if (sym == rbrace) getsym();
            else error(38); // lack '{'

            if (sym == whilesym) getsym();
            else error(45); // lack 'while'

            if (sym == lparen) getsym();
            else error(33);   // lack '('

            memcpy(nxtlev, fsys, sizeof nxtlev);
            nxtlev[rparen] = true;
            expression(nxtlev, ptx, lev);
            gen(jeq, 0, cx1);

            if (sym == rparen) getsym();
            else error(34);
        }
        if (sym == semicolon)
            getsym();
        else error(5);
    }

    memset(nxtlev, 0, sizeof nxtlev);
    test(fsys, nxtlev, 19); // follow a wrong symbol
}

/*
 * logic or processing
 */
void cond_or(bool* fsys, int* ptx, int lev)
{
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[orsym] = true;
    cond_and(nxtlev, ptx, lev);

    while (sym == orsym)
    {
        getsym();
        cond_and(nxtlev, ptx, lev);
        gen(opr, 0, 15);    // logic or operation
    }

    test(fsys, nxtlev, 46);
}

/*
 * logic and processing
 */
void cond_and(bool* fsys, int* ptx, int lev)
{
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[andsym] = true;
    or_expr(nxtlev, ptx, lev);

    while (sym == andsym)
    {
        getsym();
        or_expr(fsys, ptx, lev);
        gen(opr, 0, 14);    // logic and operation
    }

    test(fsys, nxtlev, 47);
}

/*
 * bitwise or processing
 */
void or_expr(bool* fsys, int* ptx, int lev)
{
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[bitor] = true;
    xor_expr(nxtlev, ptx, lev);

    while (sym == bitor)
    {
        getsym();
        xor_expr(fsys, ptx, lev);
        gen(opr, 0, 11);    // bitwise or operation
    }
}

/*
 * bitwise xor processing
 */
void xor_expr(bool* fsys, int* ptx, int lev)
{
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[xor] = true;
    and_expr(nxtlev, ptx, lev);

    while (sym == xor)
    {
        getsym();
        and_expr(fsys, ptx, lev);
        gen(opr, 0, 13);    // bitwise xor operation
    }
}

/*
 * bitwise and processing
 */
void and_expr(bool* fsys, int* ptx, int lev)
{
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[bitand] = true;
    equal_expr(nxtlev, ptx, lev);

    while (sym == bitand)
    {
        getsym();
        equal_expr(fsys, ptx, lev);
        gen(opr, 0, 10);    // bitwise and operation
    }
}

/*
 * equal condition processing
 */
void equal_expr(bool* fsys, int* ptx, int lev)
{
    SYMBOL eqlop;
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[eql] = true;
    nxtlev[neq] = true;
    rel_expr(nxtlev, ptx, lev);

    while (sym == eql || sym == neq)
    {
        eqlop = sym;
        getsym();
        rel_expr(nxtlev, ptx, lev);
        switch(eqlop)
        {
            case eql:   // equal judgement
                gen(opr, 0, 7);
                break;
            case neq:   // not equal judgement
                gen(opr, 1, 7);
                break;
            default: break;
        }
    }
    test(fsys, nxtlev, 47);
}

/*
 * comparation condition processing
 */
void rel_expr(bool* fsys, int* ptx, int lev)
{
    SYMBOL relop;
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[lss] = true;
    nxtlev[leq] = true;
    nxtlev[gtr] = true;
    nxtlev[geq] = true;
    shift_expr(nxtlev, ptx, lev);

    while (sym == lss || sym == leq || sym == gtr || sym == geq)
    {
        relop = sym;
        getsym();
        shift_expr(nxtlev, ptx, lev);
        switch(relop)
        {
            case lss:
                gen(opr, 0, 8);
                break;
            case leq:
                gen(opr, 1, 8);
                break;
            case gtr:
                gen(opr, 0, 9);
                break;
            case geq:
                gen(opr, 1, 9);
                break;
            default: break;
        }
    }
    test(fsys, nxtlev, 47);
}

/*
 * bitshift expression processing
 */
void shift_expr(bool* fsys, int* ptx, int lev)
{
    SYMBOL eqlop;
    bool nxtlev[N_SYM];

    memcpy(nxtlev, fsys, sizeof nxtlev);
    nxtlev[shl] = true;
    nxtlev[shr] = true;
    add_expr(nxtlev, ptx, lev);

    while (sym == shl || sym == shr)
    {
        eqlop = sym;
        getsym();
        add_expr(nxtlev, ptx, lev);
        switch(eqlop)
        {
            case shl:   // left shift
                gen(opr, 0, 16);
                break;
            case shr:   // right shift
                gen(opr, 1, 16);
                break;
            default: break;
        }
    }
}

/*
 * addition expression processing
 */
void add_expr(bool* fsys, int* ptx, int lev)
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
    nxtlev[mod] = true;
    factor(nxtlev, ptx, lev);

    while (sym == times || sym == slash || sym == mod)
    {
        mulop = sym;
        getsym();
        /* factor parsing */
        factor(nxtlev, ptx, lev);

        switch(mulop)
        {
            case times:     // generate times instruction
                gen(opr, 0, 4);
                break;
            case slash:     // generate slash instruction
                gen(opr, 0, 5);
                break;
            case mod:       // generate mod instruction
                gen(opr, 1, 5);
                break;
            default: break;
        }
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
        /* unary operation nesting */
        if (sym == notsym)
        {
            getsym();
            factor(fsys, ptx, lev);
            gen(lit, 0, 0);
            gen(opr, 0, 7);
        }
        /* the factor is a identity type */
        else if (sym == ident)
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
            /* a post-autoincrement | post-autodecrement variable factor */
            if (sym == autoincre || sym == autodecre)
            {
                int op = sym == autoincre ? 2 : 3;
                /* post-autoincrement | post-autodecrement left a value before processing to operate */
                gen(lod, lev-table[i].level, table[i].adr);
                /* do variable increment */
                gen(lit, 0, 1);
                gen(opr, 0, op);
                gen(sto, lev-table[i].level, table[i].adr);
                getsym();
            }
        }
        /* a pre-autoincrement | pre-autodecrement variable factor */
        else if (sym == autoincre || sym == autodecre)
        {
            int op = sym == autoincre ? 2 : 3;
            getsym();
            if (sym == ident)
            {
                getsym();
                i = position(id, *ptx);
                if (i == 0) error(11);
                else
                {
                    if (table[i].kind == variable)
                    {
                        /* load variable's value */
                        gen(lod, lev-table[i].level, table[i].adr);
                        /* do variable increment */
                        gen(lit, 0, 1);
                        gen(opr, 0, op);
                        gen(sto, lev-table[i].level, table[i].adr);
                        /* load variable's new value */
                        gen(lod, lev-table[i].level, table[i].adr);
                    }
                }
            }
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
