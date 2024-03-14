#include <string.h>

#include "commlinks.h"

void CLInitCommLink(CommLink* link){
    memset(link, 0, sizeof(CommLink));
    RBInit(&link->pendingRead);
    RBInit(&link->pendingWrite);
}