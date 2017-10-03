#ifndef ERRORTYPES_H_INCLUDES
#define ERRORTYPES_H_INCLUDES

#include <cstring>

using namespace std;

char ERR_TP[100][100];

void init_errors()
{
	strcmp(ERR_TP[4], "declaration lacks identity");
    strcmp(ERR_TP[5], "declaration has no primary token ';'");
    strcmp(ERR_TP[6], "a wrong statement start token or function declaration");
    strcmp(ERR_TP[7], "lack of a statement start token");
    strcmp(ERR_TP[8], "a wrong follow token after statement");
    strcmp(ERR_TP[11], "a no-declaration identity");
    strcmp(ERR_TP[12], "a variable type need for lhs");
    strcmp(ERR_TP[30], "too long for a number");
    strcmp(ERR_TP[34], "function declaration needs '()' token");
    strcmp(ERR_TP[35], "block needs start after '{' token");
    strcmp(ERR_TP[36], "block needs ending with '}' token");
    strcmp(ERR_TP[60], "program is too long --end");
    strcmp(ERR_TP[61], "Displacement address is too big --end");
}

#endif
