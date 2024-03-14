#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

int main(){
    char buffer[256]; // Buffer to store user input

    // Infinite loop to continuously read input until "exit" is entered
    while (1) {
        // Prompt the user to enter a line of text
        printf("Enter a line of text (or type 'exit' to quit): ");
        
        // Read a line of text from the user
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input.\n");
            exit(EXIT_FAILURE);
        }

        // Remove the trailing newline character, if present
        char *newline = strchr(buffer, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        // Check if the user wants to exit
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        
        Parser* p = ParserCreate();
        if(p==NULL){
            printf("No commands detected!\n");
            continue;
        }

        printf("Parser Parsing start.\n");
        if(p->Parse(p, buffer)){
            printf("True?\n");
        } else {
            printf("False?\n");
        }
        printf("Parser Parsing end.\n");
        if(p->GetTokenCount(p)==0){
            printf("Syntax error?!?!?!?\n");
        }
        ParserDestroy(p);
        printf("Parser end.\n");
    }
    return 0;
}