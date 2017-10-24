#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include <string.h>

int main(int argc, char* argv[])
{
    /* loading the input & output files */
    char fname[LEN_ID];
    #ifdef __DEBUG__
    printf("Input file name:\t");
    scanf("%s", fname);
    #else
    strcpy(fname, argv[1]);
    #endif
    if ((fin = fopen(fname, "r")) == NULL)
    {
        puts("cannot read the code file");
        exit(1);
    }
    parsing();
    processing();
    return 0;
}
