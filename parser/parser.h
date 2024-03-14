#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdint.h>

#include "../list/list.h"



typedef struct Parser Parser;
struct Parser{
    List* _tokens;

    char*       (*GetTokenAt)       (Parser*, uint64_t);
    char*       (*GetStringTokenAt) (Parser*, uint64_t);
    int32_t     (*GetInt32TokenAt)  (Parser*, uint64_t);
    long        (*GetLongTokenAt)   (Parser*, uint64_t);
    float       (*GetFloatTokenAt)  (Parser*, uint64_t);
    uint64_t    (*GetTokenCount)    (Parser*);
    uint8_t     (*IsNuberAt)        (Parser*, uint64_t);
    uint8_t     (*Parse)            (Parser*, const char*);
};

Parser* ParserCreate();
void ParserDestroy(Parser* parser);


#endif
