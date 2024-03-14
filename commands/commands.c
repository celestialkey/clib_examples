#define DEBUG
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "commands.h"

#ifdef DEBUG
#include <stdio.h>
#define DPRINTF(...) {printf(__VA_ARGS__); fflush(stdout);}
#else
#define DPRINTF(...)
#endif


// CommandParameterDesc functions
// TODO: Destroy CommandParameterDesc
CommandParameterDesc* CommandParameterDescCreate(CommandParameterType paramType, char* description) {
    CommandParameterDesc* cmdParamDesc = (CommandParameterDesc*)malloc(sizeof(CommandParameterDesc));
    if (cmdParamDesc == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    cmdParamDesc->paramType         = paramType;
    cmdParamDesc->paramDesc         = strdup(description);
    cmdParamDesc->paramConditions   = (Vector*)malloc(sizeof(Vector));
    if(cmdParamDesc->paramConditions == NULL) {
        free(cmdParamDesc);
        return NULL;
    }
    VectorInit(cmdParamDesc->paramConditions);
    return cmdParamDesc;
}
void CommandParameterDescDestroy(CommandParameterDesc* cmdParamDesc){
    if(cmdParamDesc == NULL) return;
    if(cmdParamDesc->paramDesc) free(cmdParamDesc->paramDesc);
    if(cmdParamDesc->paramConditions != NULL){
        VectorFree(cmdParamDesc->paramConditions, NULL);
        free(cmdParamDesc->paramConditions);
    }
    free(cmdParamDesc);
}
CommandParameterType CommandParameterDescGetType(CommandParameterDesc* cmdParamDesc) {
    return cmdParamDesc->paramType;
}
const char* CommandParameterDescGetDescription(CommandParameterDesc* cmdParamDesc) {
    return cmdParamDesc->paramDesc;
}
uint8_t CommandParameterDescHasConditions(CommandParameterDesc* cmdParamDesc) {
    return (VectorSize(cmdParamDesc->paramConditions) > 0) ? 1 : 0;
}
void CommandParameterDescAddCondition(CommandParameterDesc* cmdParamDesc, CommandParameterCondition* cmdCond) {
    VectorPush_back(cmdParamDesc->paramConditions, (void*)cmdCond);
}
uint16_t CommandParameterDescGetConditionCount(CommandParameterDesc* cmdParamDesc) {
    return (uint16_t)VectorSize(cmdParamDesc->paramConditions);
}
CommandParameterCondition* CommandParameterDescGetCondition(CommandParameterDesc* cmdParamDesc, uint32_t index) {
    return VectorAt(cmdParamDesc->paramConditions, index);
}


// CommandParameter functions
CommandParameter* CommandParameterCreate(CommandParameterType paramType, void* pIn, uint32_t length) {
    CommandParameter* cmdParam = (CommandParameter*)malloc(sizeof(CommandParameter));
    if (cmdParam == NULL) { return NULL; }
    cmdParam->paramType     = paramType;
    //cmdParam->paramFreeFunc = freeFunc;

    // Check to see if NULL or 0 was passed in.
    if(length == 0 || pIn == NULL){
        cmdParam->paramSize = 0;
        cmdParam->data      = NULL;
        return cmdParam;
    }

    cmdParam->data = malloc(length);
    if (cmdParam->data == NULL) {
        // Handle memory allocation failure
        free(cmdParam);
        return NULL;
    }
    memcpy(cmdParam->data, pIn, length);
    cmdParam->paramSize = length;
    return cmdParam;
}
void CommandParameterDestroy(CommandParameter* cmdParam) {
    if (cmdParam != NULL) {
        if (cmdParam->data != NULL) {
            //if(cmdParam->paramFreeFunc) cmdParam->paramFreeFunc(cmdParam->data);
            //else free(cmdParam->data);
            free(cmdParam->data);
        }
        free(cmdParam);
    }
}
CommandParameterType CommandParameterGetParameterType(CommandParameter* cmdParam) {
    return cmdParam->paramType;
}
uint8_t CommandParameterGetValue(CommandParameter* cmdParam, void* pOut) {
    memcpy(pOut, cmdParam->data, cmdParam->paramSize);
}
uint8_t CommandParameterGetData(CommandParameter* cmdParam, void* pOut, uint32_t length) {
    if (cmdParam->data == NULL || cmdParam->paramSize == 0) {
        DPRINTF("CommandParameterGetData(): No data available.\n");
        return 0;
    }
    if (pOut == NULL || length == NULL) {
        DPRINTF("CommandParameterGetData(): Invalid output buffer or length pointer.\n");
        return 0;
    }
    uint32_t localSize = CommandParameterGetSize(cmdParam);
    uint32_t readLength = (length > localSize)?localSize:length;
    memcpy(pOut, cmdParam->data, readLength);
    return 1;
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt8(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(int8_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt8(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(uint8_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt16(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(int16_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt16(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(uint16_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt32(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(int32_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt32(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(uint32_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt64(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(int64_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt64(CommandParameter* cmdParam, void* pOut) {
    return CommandParameterGetData(cmdParam, pOut, sizeof(uint64_t));
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetString(CommandParameter* cmdParam, void* pOut) {
    if (pOut == NULL) {
        DPRINTF("CommandParameterGetString(): Invalid out pointer.\n");
        return 0;
    }
    // Extract string length
    uint16_t stringLength;
    memcpy(&stringLength, cmdParam->data, sizeof(stringLength));
    // Move data pointer to string content
    char* stringData = (char*)cmdParam->data + sizeof(stringLength);
    // Copy string content to output buffer
    memcpy(pOut, stringData, stringLength);
    return 1;
}
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetBlob(CommandParameter* cmdParam, void* pOut) {
    if (pOut == NULL) {
        DPRINTF("CommandParameterGetBlob(): Invalid out pointer.\n");
        return 0;
    }
    // Extract blob length
    uint32_t blobLength;
    memcpy(&blobLength, cmdParam->data, sizeof(blobLength));
    // Move data pointer to blob content
    char* blobData = (char*)cmdParam->data + sizeof(blobLength);
    // Copy blob content to output buffer
    memcpy(pOut, blobData, blobLength);
    return 1;
}
void* CommandParameterGetPointer(CommandParameter* cmdParam){
    return &cmdParam->data;
}
uint8_t CommandParameterSetData(CommandParameter* cmdParam, void* pIn, uint32_t length) {
    if(cmdParam->data != NULL) {
        DPRINTF("CommandParameterSetData(): Attempted to write %hu over existing value.\n", cmdParam->paramType);
        return;
    }
    cmdParam->data = malloc(length);
    cmdParam->paramSize = length;
    memcpy(cmdParam->data, pIn, length);
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt8(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(int8_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt8(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(uint8_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt16(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(int16_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt16(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(uint16_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt32(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(int32_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt32(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(uint32_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt64(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(int64_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt64(CommandParameter* cmdParam, void* pIn){
    return CommandParameterSetData(cmdParam, pIn, sizeof(uint64_t));
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetString(CommandParameter* cmdParam, void* pIn, uint32_t length){
    if(cmdParam->data != NULL) {
        DPRINTF("CommandParameterSetString(): Attempted to write %hu over existing value.\n", cmdParam->paramType);
        return 0;
    }
    if(length > USHRT_MAX){
        DPRINTF("CommandParameterSetString(): String exceeds max length. Requested write size %u.\n", length);
        return 0;
    }
    uint16_t stringLength = (uint16_t)length;
    cmdParam->paramSize = length + sizeof(stringLength);
    cmdParam->data = malloc(cmdParam->paramSize);
    // Write length of the string
    memcpy(cmdParam->data, &stringLength, sizeof(stringLength));
    // Write the string
    memcpy(cmdParam->data, pIn, length);
    return 1;
}
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetBlob(CommandParameter* cmdParam, void* pIn, uint32_t length){
    if(cmdParam->data != NULL) {
        DPRINTF("CommandParameterSetBlob(): Attempted to write %hu over existing value.\n", cmdParam->paramType);
        return 0;
    }
    if(length > USHRT_MAX){
        DPRINTF("CommandParameterSetBlob(): Blob exceeds max length. Requested write size %d.\n", length);
        return 0;
    }
    uint32_t blobLength = length;
    cmdParam->paramSize = length + sizeof(blobLength);
    cmdParam->data = malloc(cmdParam->paramSize);
    // Write length of the string
    memcpy(cmdParam->data, &blobLength, sizeof(blobLength));
    // Write the string
    memcpy(cmdParam->data, pIn, length);
    return 1;
}
uint32_t CommandParameterGetSize(CommandParameter* cmdParam) {
    return cmdParam->paramSize;
}
CommandParameter* CommandParameterClone(CommandParameter* cmdParam){
    if(cmdParam == NULL) return NULL;
    CommandParameter* pParam = CommandParameterCreate(cmdParam->paramType, cmdParam->data, cmdParam->paramSize);
    if(pParam == NULL) return NULL;
    return pParam;
}


// CommandDesc functions
CommandDesc* CommandDescCreate(uint16_t cmdId, const char* name, const char* desc, uint32_t flags) {
    CommandDesc* cmdDesc = (CommandDesc*)malloc(sizeof(CommandDesc));
    if (cmdDesc == NULL) {
        // Handle memory allocation failure
        return NULL;
    }
    cmdDesc->cmdId          = cmdId;
    cmdDesc->cmdName        = strdup(name);
    cmdDesc->cmdDesc        = strdup(desc);
    cmdDesc->cmdFlags       = flags;
    cmdDesc->cmdFlags       = 0;
    //cmdDesc->cmdFreeFunc    = freeFunc;
    cmdDesc->cmdParamDescs  = (Vector*)malloc(sizeof(Vector));
    if (cmdDesc->cmdParamDescs == NULL) {
        free(cmdDesc->cmdName);
        free(cmdDesc->cmdDesc);
        return NULL;
    }
    VectorInit(cmdDesc->cmdParamDescs);
    return cmdDesc;
}
void CommandDescDestroy(CommandDesc* cmdDesc) {
    if (cmdDesc != NULL) {
        if (cmdDesc->cmdName != NULL) {
            free(cmdDesc->cmdName);
        }
        if (cmdDesc->cmdDesc != NULL) {
            free(cmdDesc->cmdDesc);
        }
        VectorFree(cmdDesc->cmdParamDescs, NULL);
        free(cmdDesc->cmdParamDescs);
        free(cmdDesc);
    }
}
void CommandDescAddParamDesc(CommandDesc* cmdDesc, CommandParameterDesc* pParam) {
    VectorPush_back(cmdDesc->cmdParamDescs, pParam);
}
uint8_t CommandDescIsFlagSet(CommandDesc* cmdDesc, uint32_t flag) {
    return (cmdDesc->cmdFlags & flag) ? 1 : 0;
}
uint16_t CommandDescGetID(CommandDesc* cmdDesc) {
    return cmdDesc->cmdId;
}
const char* CommandDescGetName(CommandDesc* cmdDesc) {
    return cmdDesc->cmdName;
}
const char* CommandDescGetDesc(CommandDesc* cmdDesc) {
    return cmdDesc->cmdDesc;
}
CommandParameterDesc* CommandDescGetParameterDesc(CommandDesc* cmdDesc, uint32_t index) {
    return (CommandParameterDesc*)VectorAt(cmdDesc->cmdParamDescs, index);
}
uint32_t CommandDescGetParameterCount(CommandDesc* cmdDesc) {
    return VectorSize(cmdDesc->cmdParamDescs);
}
CommandParameterType CommandDescGetParameterType(CommandDesc* cmdDesc, uint32_t index) {
    CommandParameterDesc* cmdParam = (CommandParameterDesc*)VectorAt(cmdDesc->cmdParamDescs, index);
    if(cmdParam == NULL) return CPT_NONE;
    return cmdParam->paramType;
}
CommandDesc* CommandDescClone(CommandDesc* cmdDesc) {
    // Implementation for cloning a command description
    // TODO: I really don't want to do this...
    return NULL;
}

// Command functions
Command* CommandCreateFromDesc(CommandDesc* cmdDesc) {
    Command* cmd = NULL;
    CommandReset(cmd);
    if(cmd==NULL) return NULL;

    CommandSetIDByCommandDesc(cmd, cmdDesc);
    //cmd->cmdFreeFunc = freeFunc;
    return cmd;
}
void CommandDestroy(Command* cmd){
    if(cmd==NULL) return;
    if(cmd->cmdDesc != NULL) CommandDescDestroy(cmd->cmdDesc);
    VectorFree(cmd->cmdParams, NULL);
    free(cmd->cmdParams);
    free(cmd);
}
void CommandReset(Command* cmd){
    CommandDestroy(cmd);
    
    cmd = (Command*)malloc(sizeof(Command));
    if(cmd == NULL){
        return NULL;
    }
    cmd->cmdDesc = NULL;
    cmd->cmdParams = (Vector*)malloc(sizeof(Vector));
    if(cmd->cmdParams == NULL){
        free(cmd);
        return NULL;
    }
    VectorInit(cmd->cmdParams);
}
Command* CommandCreate(uint16_t cmdId, CommandManager* cmdMgrRef) {
    Command* cmd = NULL;
    CommandReset(cmd);
    cmd->cmdDesc = CommandManagerGetCommandDescById(cmdMgrRef, cmdId);
    //cmd->cmdFreeFunc = freeFunc;
    return NULL;
}
uint8_t CommandAddParameter(Command* cmd, CommandParameter* param) {
    if(param == NULL || cmd == NULL) return 0;
    // Do I need to add a VectorReserve(Vector* vec);?
    // Implementation for adding a parameter to a command
    if(VectorCapacity(cmd->cmdParams) == CommandGetParameterCount(cmd)) 
        return 0;
    int count = VectorSize(cmd->cmdParams);
    int paramDescCount = CommandGetParameterCount(cmd);
    // Assert? if(count < paramDescCount){} // TODO
    CommandParameterDesc* pParamDesc = CommandDescGetParameterDesc(cmd->cmdDesc, count);
    if(CommandParameterGetParameterType(param) != CommandParameterDescGetType(pParamDesc)){
        return 0;
    }
    VectorPush_back(cmd->cmdParams, param);
    return 1;
}
uint8_t CommandGetParameterValue(Command* cmd, void* pOut, CommandParameterType paramType, uint32_t index) {
    if(pOut == NULL) return 0;

    CommandParameter* param = CommandGetParameter(cmd, index);
    if(param == NULL) return 0;

    if(CommandParameterGetParameterType(param) != paramType) return 0;
    if(paramType == CPT_STR && CommandParameterGetSize(param)>=0){
        char* strPtr = *(char**)CommandParameterGetPointer(param);
        int len = strlen(strPtr);
        // TODO: Double check!
        printf("Might crash here...\n");
        fflush(stdout);
        printf("Get [%s]...\n", strPtr);
        fflush(stdout);
        strncpy((char*)pOut, strPtr, len);
        ((char*)pOut)[len-1]=0;
    } else {
        CommandParameterGetValue(param, pOut);
    }
    return 1;
}
uint8_t CommandGetData(Command* cmd, void* pOut, uint32_t length){
    if(pOut == NULL) return 0;
    int paramCount = CommandGetParameterCount(cmd);
    uint16_t dataCount = sizeof(uint16_t);

    // Write the command id
    uint16_t cmdId = CommandDescGetID(cmd->cmdDesc);
    memcpy(pOut+dataCount, &(cmdId), sizeof(cmdId));
    dataCount += sizeof(cmdId);
    // Dump parameters to buffer
    for(int i=0; i<VectorSize(cmd->cmdParams); i++){
        CommandParameter* param = CommandGetParameter(cmd, i);
        dataCount += CommandParameterGetData(param, pOut+dataCount, length-dataCount);
    }
    // Write the size to the start of the buffer
    memcpy(pOut, &dataCount, sizeof(dataCount));
    return 1;
}
CommandParameter* CommandGetParameter(Command* cmd, uint32_t index) {
    if(index < 0 || index >= VectorSize(cmd->cmdParams)) return NULL;
    return VectorAt(cmd->cmdParams, index);
}
uint32_t CommandGetParameterCount(Command* cmd) {
    // Implementation for getting the parameter count in a command
    return VectorSize(cmd->cmdParams);
}
CommandDesc* CommandGetDescription(Command* cmd) {
    return cmd->cmdDesc;
}
uint8_t CommandIsFlagSet(Command* cmd, uint32_t flag) {
    return CommandDescIsFlagSet(cmd->cmdDesc, flag);
}
Command* CommandClone(Command* cmd) {
    if(cmd == NULL) return NULL;
    if(cmd->cmdDesc == NULL) return NULL;

    Command* pClone = CommandCreateFromDesc(cmd->cmdDesc);
    if(pClone == NULL) return NULL;

    int paramCount = CommandGetParameterCount(cmd);
    for(int i=0; i<paramCount; i++){
        CommandParameter* pParam = CommandGetParameter(cmd, i);
        if(!CommandAddParameter(pClone, CommandParameterClone(pParam))){
            free(pClone);
            return NULL;
        }
    }
    return pClone;
}
void CommandSetIDByCommandDesc(Command* cmd, CommandDesc* cmdDesc){
    if(cmdDesc == NULL){
        DPRINTF("CommandSetIDByCommandDesc(): NULL CommandDesc\n");
        return;
    }
    cmd->cmdDesc = cmdDesc;
    if(!VectorReserve(cmd->cmdParams, CommandDescGetParameterCount(cmd->cmdDesc))){
        DPRINTF("CommandSetIDByCommandDesc(): Failed to reserve!\n");
    }
}
void CommandSetIDByIDLookup(Command* cmd, CommandManager* cmdMgr, uint16_t cmdId){
    cmd->cmdDesc = CommandManagerGetCommandDescById(cmdMgr, cmdId);
    if(cmd->cmdDesc == NULL){
        DPRINTF("CommandSetIDByIDLookup(): Command lookup failed.\n");
        return;
    }
    if(!VectorReserve(cmd->cmdParams, CommandDescGetParameterCount(cmd->cmdDesc))){
        DPRINTF("CommandSetIDByIDLookup(): Failed to reserve!\n");
        return;
    }
}
uint8_t CommandSetData(Command* cmd, void* pIn, CommandManager* cmdMgr, uint16_t expectedSize){
    CommandReset(cmd);
    uint16_t dataCount = 0;
    uint16_t totalSize = 0;

    // Read in the total size
    memcpy(&totalSize, pIn, sizeof(uint16_t));
    if((expectedSize != USHRT_MAX) && (expectedSize != totalSize)) return 0;
    dataCount += sizeof(totalSize);

    // Read in the command id
    uint16_t cmdId = 0;
    memcpy(&totalSize, pIn+dataCount, sizeof(cmdId));
    dataCount += sizeof(cmdId);

    // Get the command prototype
    CommandDesc* cmdDesc = CommandManagerGetCommandDescById(cmdMgr, cmdId);
    if(cmdDesc == NULL) {
        DPRINTF("CommandSetData(): Failed lookup by id.\n");
        return 0;
    }
    CommandSetIDByCommandDesc(cmd, cmdDesc);
    uint32_t paramCount = CommandDescGetParameterCount(cmdDesc);
    for(int i=0; i<paramCount; i++){
        CommandParameterType pType = CommandDescGetParameterType(cmdDesc, i);
        CommandParameter* param = CommandParameterCreate(pType, NULL, 0);
        switch(pType){
            case CPT_STR: {
                uint16_t sizeCheck = 0;
                memcpy(&sizeCheck, pIn+dataCount, sizeof(sizeCheck));
                if(sizeCheck > expectedSize || sizeCheck == 0){
                    DPRINTF("CommandSetData(): Stored string reports size %hu and surpasses total size expected %hu.\n", sizeCheck, expectedSize);
                    return 0;
                }
                if(!CommandParameterSetString(param, pIn+dataCount, sizeCheck)){
                    return 0;
                }
                dataCount += sizeCheck + sizeof(sizeCheck);
                break;
            }
            case CPT_BLOB: {
                uint32_t sizeCheck = 0;
                memcpy(&sizeCheck, pIn+dataCount, sizeof(sizeCheck));
                if(sizeCheck > expectedSize || sizeCheck == 0){
                    DPRINTF("CommandSetData(): Stored blob reports size %hu and surpasses total size expected %hu.\n", sizeCheck, expectedSize);
                    return 0;
                }
                if(!CommandParameterSetBlob(param, pIn, sizeCheck)){
                    return 0;
                }
                dataCount += sizeCheck + sizeof(sizeCheck);
                break;
            }
            case CPT_BIT:{
                DPRINTF("CommandSetData(): CPT_BIT not implemented!\n");
                return 0;
            }
            case CPT_BOOL:{
                DPRINTF("CommandSetData(): CPT_BOOL not implemented!\n");
                return 0;
            }
            case CPT_INT:{
                if(!CommandParameterSetInt32(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(int32_t);
                break;
            }
            case CPT_UINT:{
                if(!CommandParameterSetUInt32(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(uint32_t);
                break;
            }
            case CPT_CHAR:{
                if(!CommandParameterSetInt8(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(int8_t);
                break;
            }
            case CPT_UCHAR:{
                if(!CommandParameterSetUInt8(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(uint8_t);
                break;
            }
            case CPT_FLOAT:{
                DPRINTF("CommandSetData(): CPT_FLOAT not implemented!\n");
                return 0;
            }
            case CPT_INT64:{
                if(!CommandParameterSetInt64(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(int64_t);
                break;
            }
            case CPT_UINT64:{
                if(!CommandParameterSetUInt64(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(uint64_t);
                break;
            }
            case CPT_USHORT:{
                if(!CommandParameterSetUInt16(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(uint16_t);
                break;
            }
            case CPT_SHORT:{
                if(!CommandParameterSetInt16(param, pIn)){
                    return 0;
                }
                dataCount += sizeof(int16_t);
                break;
            }
            default:{
                DPRINTF("CommandSetData(): Unknown parameter type encountered. [%hu].", pType);
                return 0;
            }
        }
        if(!VectorPush_back(cmd->cmdParams, param)){
            DPRINTF("CommandSetData(): Push_back failed when adding to vector item of type %hu!\n", pType);
            return 0;
        }
        if(expectedSize > dataCount){
            DPRINTF("CommandSetData(): Exceeded expected size!\n");
            return 0;
        }
    }
    if(expectedSize != dataCount){
        DPRINTF("CommandSetData(): Missing data from source!\n");
        return 0;
    }
    return 1;
}
uint32_t CommandGetSize(Command* cmd){
    if(cmd->cmdDesc == NULL) return 0;
    uint32_t size = 0;
    // size + command id
    size = sizeof(uint32_t) + sizeof(uint16_t);
    
    int paramCount = CommandGetParameterCount(cmd);
    for(int i=0; i<paramCount; i++){
        CommandParameter* p = CommandGetParameter(cmd, i);
        size += CommandParameterGetSize(p);
    }
    return size;
}

// CommandManager functions
CommandManager* CommandManagerCreate() {
    CommandManager* cmdMgr = (CommandManager*)malloc(sizeof(CommandManager));
    if(!cmdMgr){
        DPRINTF("CommandManagerCreate(): Failed malloc\n");
        return NULL;
    }
    return cmdMgr;
}

void CommandManagerDestroy(CommandManager* cmdMgr) {
    if(!cmdMgr) return;
    // TODO: This needs help
    while(CommandManagerPeekCommand(cmdMgr)){
        free(CommandManagerGetCommand(cmdMgr));
    }
    free(cmdMgr);
}

uint32_t CommandManagerGetCommandDescCount(CommandManager* cmdMgr) {
    return (uint32_t)MapSize(cmdMgr->CommandDescMap);
}

uint32_t CommandManagerGetCommandQueueCount(CommandManager* cmdMgr) {
    List* cmdList = cmdMgr->CommandList;
    return (uint32_t)cmdList->Size(cmdList);
}

CommandDesc* CommandManagerGetCommandDesc(CommandManager* cmdMgr, uint32_t index) {
    if(index < 0 || index >= MapSize(cmdMgr->CommandDescMap)) return NULL;
    MapGetKey_Int32(index);
    DPRINTF("CommandManagerGetCommandDesc(): TODO: Not sure how this should work with a MAP... Maybe a Queue or List is better fit?\n");
    return NULL;
}

CommandDesc* CommandManagerGetCommandDescById(CommandManager* cmdMgr, uint16_t cmdId) {
    KeyValuePair* kp = MapFind(cmdMgr->CommandDescMap, cmdId);
    if(kp == NULL) return NULL;
    return (CommandDesc*)kp->second;
}

uint8_t CommandManagerAddCommandDesc(CommandManager* cmdMgr, CommandDesc* cmdDesc) {
    if(MapFind(cmdMgr->CommandDescMap,cmdDesc->cmdId)) {
        DPRINTF("CommandManagerAddCommandDesc(): Attempted to add existing CommandDesc\n");
        return 0;
    }
    uint64_t key = MapGetKey_UInt16(cmdDesc->cmdId);
    if(!MapAdd(cmdMgr->CommandDescMap, key, cmdDesc)){
        DPRINTF("CommandManagerAddCommandDesc(): Failed attempt to add CommandDesc to map\n");
        return 0;
    }
    return 1;
}

uint8_t CommandManagerPostCommand(CommandManager* cmdMgr, Command* cmd) {
    List* cmdList = cmdMgr->CommandList;
    if(!cmdList->Push_Back(cmdList, cmd)){
        DPRINTF("CommandManagerPostCommand(): Failed to add command to list.\n");
        return 0;
    }
    return 1;
}

Command* CommandManagerGetCommand(CommandManager* cmdMgr) {
    List* cmdList = cmdMgr->CommandList;
    Command* cmd = (Command*)cmdList->Pop_Front(cmdList);
    if(cmd==NULL){
        DPRINTF("CommandManagerGetCommand(): Failed to retrieve command off list. Empty?\n");
        return NULL;
    }
    return cmd;
}

Command* CommandManagerPeekCommand(CommandManager* cmdMgr) {
    List* cmdList = cmdMgr->CommandList;
    Command* cmd = (Command*)cmdList->Front(cmdList);
    if(cmd==NULL){
        DPRINTF("CommandManagerPeekCommand(): Failed to retrieve command off list. Empty?\n");
        return NULL;
    }
    return cmd;
}

void CommandManagerGetSyntax(char* szSyntax, const CommandDesc* pCD)
{
	sprintf(szSyntax, "%s ", CommandDescGetName(pCD));
	for(int i=0; i<CommandDescGetParameterDescCount(pCD); i++){
		CommandParameterDesc* pPD = CommandDescGetParameterDesc(pCD, i);
		sprintf(szSyntax, "%s %s", szSyntax, CommandDescGetDesc(pCD));
	}
}

uint8_t CommandManagerParseMessage(CommandManager* cmdMgr, Command* cmd, char* sErrorMsg, int errMsgMaxLen, const char* msg) {
    // Implementation for parsing a message in a command manager
    return 0;
}

void CommandManagerAddAlias(CommandManager* cmdMgr, const char* cmd, const char* alias) {
    // Implementation for adding an alias to a command manager
}