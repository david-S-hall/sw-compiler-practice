#ifndef ERRORTYPES_H_INCLUDES
#define ERRORTYPES_H_INCLUDES

#include <cstring>

using namespace std;

char ERR_TP[100][100];

void init_errors()
{
	strcmp(ERR_TP[4], "declaration needs identity symbol");
    strcmp(ERR_TP[5], "declaration has no primary symbol ';'");
    strcmp(ERR_TP[6], "a wrong statement start symbol or function declaration");
    strcmp(ERR_TP[7], "lack of a statement start symbol");
    strcmp(ERR_TP[30], "too long for a number");
    strcmp(ERR_TP[34], "function declaration needs primary symbol '()'");
    strcmp(ERR_TP[35], "block needs start lbrace '{'");
    strcmp(ERR_TP[36], "block needs ending rbrace '}'");
}

#endif
