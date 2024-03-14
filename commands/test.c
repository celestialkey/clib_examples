#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "commands.h"
#include "../linkedlists/linkedlists.h"

#define MAX_ARGS 10

uint8_t MatchCommand(LLNode* cmdList){
    return 1;
}

uint8_t ParseInput(char* userinput){
    char* token = strtok(userinput, " \n\r");
    LLNode* cmdNode = NULL;
    while(token != NULL){
        /*
        if(cmdNode==NULL){ cmdNode = LLCreateNode(strdup(token)); }
        else{ LLInsertAtEnd(cmdNode, strdup(token)); }*/
        token = strtok(NULL, " \n\r");
    }
    return MatchCommand(cmdNode);
}

int main(){
    char rawinput[128];
    while(1){
        memset(rawinput, 0, sizeof(rawinput));
        fgets(rawinput, sizeof(rawinput), stdin);
        rawinput[sizeof(rawinput)-1] = 0;

        if(!ParseInput(rawinput))
            break;
    }
    return 0;
}