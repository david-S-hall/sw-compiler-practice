#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"

void parsing();
void processing();

int main()
{
    /* loading the input & output files */
    char fname[LEN_ID];
    printf("Input file name:\t");
    scanf("%s", fname);

    if ((fin = fopen(fname, "r")) == NULL)
    {
        puts("cannot read the code file");
        exit(1);
    }

    fresult = fopen("fresult.txt", "w");
    ferr = fopen("ferr.txt", "w");
    ftable = fopen("ftable.txt", "w");
    fcode = fopen("fcode.txt", "w");
    foutput = fopen("foutput.txt", "w");

    parsing();

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
