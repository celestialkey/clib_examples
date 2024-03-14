#include <string.h>
#include <stdlib.h>

#include "parser.h"


#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(...) {printf(__VA_ARGS__); fflush(stdout);}
#else
#define DPRINTF(...)
#endif

/*
struct Parser{
    List* _tokens;

    char*       (*GetTokenAt)       (Parser*, uint64_t);
    char*       (*GetStringTokenAt) (Parser*, uint64_t);
    int32_t     (*GetInt32TokenAt)  (Parser*, uint64_t);
    long        (*GetLongTokenAt)   (Parser*, uint64_t);
    float       (*GetFloatTokenAt)  (Parser*, uint64_t);
    uint64_t    (*GetTokenCount)    (Parser*);
    uint8_t     (*IsNuberAt)        (Parser*, uint64_t);
};*/
static uint8_t GetNewToken(const char* rCmd, char* dstTokenBuffer, const char* delimiters){
    static char tmpToken[256]="";
    if(rCmd != NULL){
        strncpy(tmpToken, rCmd, 256);
    }

    
    int rCmdLength = strlen(tmpToken);
    if(rCmdLength==0){
        DPRINTF("GetNewToken(): Source command length is 0.\n");
        return 0;
    }
    DPRINTF("GetNewToken(): Parsing (%d)[%s].\n", rCmdLength, tmpToken);

    int delimiterCount = strlen(delimiters); 
    for(int i=0; i<rCmdLength; i++){
        for(int j=0; j<delimiterCount; j++){
            //DPRINTF("GetNewToken(): Character[%c][%d] Delimiter(%c).\n", tmpToken[i],i,delimiters[j]);
            if(tmpToken[i] == delimiters[j]){
                if(i==0){
                    dstTokenBuffer[0] = tmpToken[i];
                    dstTokenBuffer[1] = 0;
                    DPRINTF("GetNewToken(): Pre-Strcpy [%s].\n", tmpToken);
                    strcpy(tmpToken, tmpToken+1);
                    DPRINTF("GetNewToken(): Post-Strcpy [%s].\n", tmpToken);
                } else {
                    DPRINTF("GetNewToken(): Pre-Memcpy [%s].\n", dstTokenBuffer);
                    memcpy(dstTokenBuffer, tmpToken, i);
                    DPRINTF("GetNewToken(): Post-Memcpy [%s].\n", dstTokenBuffer);
                    dstTokenBuffer[i] = 0;
                    DPRINTF("GetNewToken(): Pre-Strcpy#2 [%s].\n", tmpToken);
                    strcpy(tmpToken, tmpToken+i);
                    DPRINTF("GetNewToken(): Pre-Strcpy#2 [%s].\n", tmpToken);
                }
                return 1;
            }
        }
    }
    strcpy(dstTokenBuffer, tmpToken);
    tmpToken[0] = 0;
    return 1;
}


char*   _GetTokenAt         (Parser* parser, uint64_t index){
    if(parser == NULL){
        DPRINTF("_GetTokenAt(): Null parser passed in.\n");
        return NULL;
    }
    List* l = parser->_tokens;
    return (char*)l->Get(l, index);
}
char*       _GetStringTokenAt   (Parser* parser, uint64_t index){ return _GetTokenAt(parser, index); }
int32_t     _GetInt32TokenAt    (Parser* parser, uint64_t index){ return atoi(_GetTokenAt(parser, index)); }
long        _GetLongTokenAt     (Parser* parser, uint64_t index){ return atol(_GetTokenAt(parser, index)); }
float       _GetFloatTokenAt    (Parser* parser, uint64_t index){ return atof(_GetTokenAt(parser, index)); }
uint64_t    _GetTokenCount      (Parser* parser) { return parser->_tokens->Size(parser->_tokens); }
uint8_t     _IsNumberAt         (Parser* parser, uint64_t index){
    char* pToken = _GetTokenAt(parser, index);
    if(pToken==NULL){
        DPRINTF("_IsNumberAt(): Failed to get token at index.\n");
        return 0;
    }
    int tokenLength = strlen(pToken);
    for(int i=0; i<tokenLength; i++){
        if(!(pToken[i]>='0' && pToken[i]<='9') || pToken[i]=='.'){
            return 0;
        }
    }
    return 1;
}
uint8_t     _Parse              (Parser* parser, const char* sourceMsg){
    DPRINTF("_Parse(): Entered.\n");
    char tokenDelimiters[] = " ,\t'";
    char token[256];
    if(GetNewToken(sourceMsg, token, tokenDelimiters)==0){
        DPRINTF("_Parse(): Failed locate tokens in command string.\n");
        return 1;
    }
    while(1){
        //memset(token, 0, 256); // TODO: Mystery? Remove?
        switch(token[0]){
            case ' ': break;
            case ',': break;
            case '\t': break;
            case '\'':{
                if(!GetNewToken(NULL, token, "'")){ 
                    return 1;
                }
                char* newToken = (char*)malloc(strlen(token)+1);
                if(newToken == NULL){
                    DPRINTF("_Parse(): Malloc2 failed.\n");
                    return 0;
                }
                strcpy(newToken, token);
                DPRINTF("_Parse(): #1 Pushing on list [%s]...\n", newToken);
                if(!parser->_tokens->Push_Back(parser->_tokens, newToken)){
                    DPRINTF("_Parse(): Push_Back failed.\n");
                    return 0;
                }
                if(!GetNewToken(NULL, token, "'")){ 
                    return 1; 
                }
                break;
            }
            default:{
                DPRINTF("_Parse(): Default...\n");
                char* newToken = (char*)malloc(strlen(token)+1);
                if(newToken == NULL){
                    DPRINTF("ParserCreate(): Malloc2 failed.\n");
                    return 0;
                }
                strcpy(newToken, token);
                DPRINTF("_Parse(): #2 Pushing on list [%s]...\n", newToken);
                if(!parser->_tokens->Push_Back(parser->_tokens, newToken)){
                    DPRINTF("_Parse(): Push_Back failed.\n");
                    return 0;
                }
                break;
            }
        }
        if(!GetNewToken(NULL, token, tokenDelimiters)){ return 1; }
    }
    return 1;
}


Parser* ParserCreate(){
    Parser* p = (Parser*)malloc(sizeof(Parser));
    if(p==NULL){
        DPRINTF("ParserCreate(): Malloc failed.\n");
        return NULL;
    }
    p->_tokens = ListCreate();
    if(p->_tokens == NULL){
        DPRINTF("ParserCreate(): Malloc2 failed.\n");
        free(p);
        return NULL;
    }

    p->GetStringTokenAt = _GetStringTokenAt;
    p->GetInt32TokenAt  = _GetInt32TokenAt;
    p->GetFloatTokenAt  = _GetFloatTokenAt;
    p->GetLongTokenAt   = _GetLongTokenAt;
    p->GetTokenCount    = _GetTokenCount;
    p->GetTokenAt       = _GetTokenAt;
    p->IsNuberAt        = _IsNumberAt;
    p->Parse            = _Parse;
    
    return p;
}
void ParserDestroy(Parser* parser){
    List* l = parser->_tokens;
    ListIter* iter = l->Begin(l);
    while(!iter->IsEnd(iter)){
        char* pToken = iter->current->data;
        if(pToken){
            free(pToken);
        }
        iter->Next(iter);
    }
    ListIterDestroy(iter);
    l->Clear(l);
    ListDestroy(parser->_tokens);
    free(parser);
}
