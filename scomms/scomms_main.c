#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <string.h>

#include "scomms.h"

struct {
    uint8_t mode;
} args;

int ParseArgs(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "sc"))!=-1){
        switch(opt){
            case 's':
                args.mode = 1;
                break;
            case 'c':
                args.mode = 2;
                break;
            default:
                return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    memset(&args, 0, sizeof(args));
    if(ParseArgs(argc, argv)){
        return -1;
    }
    if(args.mode == 2){
        printf("Client mode.\n");
        client();
    } else if (args.mode == 1)
    {
        printf("Server mode.\n");
        server();
    }
    
    return 0;
}