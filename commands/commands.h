#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdint.h>

#include "../map/map.h"
#include "../vector/vector.h"
//#include "../linkedlists/linkedlists.h"
#include "../list/list.h"

//typedef struct Vector Vector;   // ../vector/vector.h
//typedef struct Map Map;         // ../map/map.h
//typedef struct LLNode LLNode;   // ../linkedlists/linkedlists.h

typedef enum {
    CPT_INT		= 0,
    CPT_UINT	= 1,
    CPT_FLOAT	= 2,
    CPT_BOOL	= 3,
    CPT_STR		= 4,
    CPT_BLOB	= 10,
    CPT_CHAR	= 11,
    CPT_UCHAR	= 12,
    CPT_SHORT	= 13,
    CPT_USHORT	= 14,
    CPT_INT64	= 15,
    CPT_UINT64	= 16,
    CPT_BIT     = 18,
    CPT_END		= 19,
    CPT_NONE,
} CommandParameterType;
#define MAX_BLOB_SIZE		(0x100000)

typedef struct CommandParameterDesc CommandParameterDesc;
typedef struct CommandParameter CommandParameter;
typedef struct CommandManager CommandManager;
typedef struct CommandDesc CommandDesc;
typedef struct Command Command;

struct CommandParameterDesc{
    CommandParameterType    paramType;
    char*                   paramDesc;
    Vector*                 paramConditions;    // Vector of CommandParameterCondition's
};
struct CommandParameter{
    CommandParameterType paramType;
    uint32_t paramSize;
    void* data;
};
struct CommandDesc{
    uint16_t    cmdId;
    char*       cmdName;
    char*       cmdDesc;
    uint32_t    cmdFlags;
    Vector*     cmdParamDescs; // Vector of CommandParameterDesc's
};
struct Command{
    CommandDesc*cmdDesc;
    Vector*     cmdParams;
};
struct CommandManager {
    Map*    CommandDescMap; // uint16_t, CommandDesc*
    List*   CommandList;    // Command*
    Map*    CommandAliasMap;// String, String

    /*
    uint32_t        (*GetCommandDescCount)    (CommandManager* cmdMgr);
    uint32_t        (*GetCommandQueueCount)   (CommandManager* cmdMgr);
    CommandDesc*    (*GetCommandDesc)         (CommandManager* cmdMgr, uint32_t index);
    CommandDesc*    (*GetCommandDescById)     (CommandManager* cmdMgr, uint16_t cmdId);
    uint8_t         (*AddCommandDesc)         (CommandManager* cmdMgr, CommandDesc* cmdDesc);
    uint8_t         (*PostCommand)            (CommandManager* cmdMgr, Command* cmd);
    Command*        (*GetCommand)             (CommandManager* cmdMgr);
    Command*        (*PeekCommand)            (CommandManager* cmdMgr);
    uint8_t         (*ParseMessage)           (CommandManager* cmdMgr, Command* cmd, char* sErrorMsg, int errMsgMaxLen, const char* msg);
    void            (*AddAlias)               (CommandManager* cmdMgr, const char* cmd, const char* alias);
    // void         (*AssignDesc)             (CommandManager* cmdMgrTarget, CommandManager* cmdMgrSource);   // Is this needed?
    */
};

//============================================
//              CommandParameterCondition
//============================================
typedef struct {} CommandParameterCondition; // Not sure what to do with this at the moment




//============================================
//              CommandParameterDesc
//============================================
// Create a new CommandParameterDesc
CommandParameterDesc* CommandParameterDescCreate(CommandParameterType paramType, char* description);
// Destroy a CommandParameterDesc
void CommandParameterDescDestroy(CommandParameterDesc* cmdParamDesc);
// Retrieve the type from the CommandParameterDesc struct
CommandParameterType CommandParameterDescGetType(CommandParameterDesc* cmdParamDesc);
// Retrieve the description from the CommandParameterDesc struct
const char* CommandParameterDescGetDescription(CommandParameterDesc* cmdParamDesc);
// Check if the CommandParameterDesc struct has any conditions attached
uint8_t CommandParameterDescHasConditions(CommandParameterDesc* cmdParamDesc);
// Add a new condition to the CommandParameterDesc struct
void CommandParameterDescAddCondition(CommandParameterDesc* cmdParamDesc, CommandParameterCondition* cmdCond);
// Get the number of conditions attached to the CommandParameterDesc struct
uint16_t CommandParameterDescGetConditionCount(CommandParameterDesc* cmdParamDesc);
// Retrieve a condition from the CommandParameterDesc struct
CommandParameterCondition* CommandParameterDescGetCondition(CommandParameterDesc* cmdParamDesc, uint32_t index);




//============================================
//              CommandParameter
//============================================
// Create a new CommandParameter
CommandParameter* CommandParameterCreate(CommandParameterType paramType, void* pIn, uint32_t length);
// Destroy a CommandParameter struct
void CommandParameterDestroy(CommandParameter* cmdParam);
// Get the paramter type from the CommandParameter struct
CommandParameterType CommandParameterGetParameterType(CommandParameter* cmdParam);
// Get the value from the CommandParameter struct
uint8_t CommandParameterGetValue(CommandParameter* cmdParam, void* pOut);
// Get the value from the CommandParameter struct
void* CommandParameterGetPointer(CommandParameter* cmdParam);
// Get 'length' amount of data from the parameter
uint8_t CommandParameterGetData(CommandParameter* cmdParam, void* pOut, uint32_t length);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt8(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt8(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt16(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt16(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt32(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt32(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetInt64(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetUInt64(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetString(CommandParameter* cmdParam, void* pOut);
// Get the value from a CommandParameter struct
uint8_t CommandParameterGetBlob(CommandParameter* cmdParam, void* pOut);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetData(CommandParameter* cmdParam, void* pIn, uint32_t length);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt8(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt8(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt16(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt16(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt32(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt32(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetInt64(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetUInt64(CommandParameter* cmdParam, void* pIn);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetString(CommandParameter* cmdParam, void* pIn, uint32_t length);
// Set the value in a CommandParameter struct
uint8_t CommandParameterSetBlob(CommandParameter* cmdParam, void* pIn, uint32_t length);
// Get the size of the 'value' or data in the CommandParameter struct
uint32_t CommandParameterGetSize(CommandParameter* cmdParam);
// Clone a parameter into a new instance
CommandParameter* CommandParameterClone(CommandParameter* cmdParam);



//============================================
//              CommandDesc
//============================================
// Initialize a CommandDesc struct
CommandDesc* CommandDescCreate(uint16_t cmdId, const char* name, const char* desc, uint32_t flags);
// Destroy a CommandDesc struct
void CommandDescDestroy(CommandDesc* cmdDesc);
// Add a new description to the command description
void CommandDescAddParamDesc(CommandDesc* cmdDesc, CommandParameterDesc* pParam);
// Check if a flag is set in the struct CommandDesc
uint8_t CommandDescIsFlagSet(CommandDesc* cmdDesc,uint32_t flag);
// Get the command id from the CommandDesc struct
uint16_t CommandDescGetID(CommandDesc* cmdDesc);
// Get the command name from the CommandDesc struct
const char* CommandDescGetName(CommandDesc* cmdDesc);
// Get the command description from the CommandDesc struct
const char* CommandDescGetDesc(CommandDesc* cmdDesc);
// Retrieve a parameter description at index
CommandParameterDesc* CommandDescGetParameterDesc(CommandDesc* cmdDesc, uint32_t index);
// Retrieve the number of parameters
uint32_t CommandDescGetParameterCount(CommandDesc* cmdDesc);
// Get the type of parameter at index
CommandParameterType CommandDescGetParameterType(CommandDesc* cmdDesc, uint32_t index);
// Clone the Command
CommandDesc* CommandDescClone(CommandDesc* cmdDesc);




//============================================
//              Command
//============================================
// Create a Command | MCommand.h:100
Command* CommandCreateFromDesc(CommandDesc* cmdDesc);
Command* CommandCreate(uint16_t cmdId, CommandManager* cmdMgrRef);
// Reset the command and destroy everything inside it. (DOES NOT FREE MEMORY)
void CommandReset(Command* cmd);
// Reserve a number of parameters based off a CommandDesc reference
void CommandReserve(Command* cmd, CommandDesc* cmdDesc);
void CommandSetIDByCommandDesc(Command* cmd, CommandDesc* cmdDesc);
void CommandSetIDByIDLookup(Command* cmd, CommandManager* cmdMgr, uint16_t cmdId);
// Free all memory associated with the Command struct
void CommandDestroy(Command* cmd);
// Add parameter to a command struct
uint8_t CommandAddParameter(Command* cmd, CommandParameter* param);
// Retrieve a value from a parameter in the command
uint8_t CommandGetParameterValue(Command* cmd, void* pOut, CommandParameterType paramType, uint32_t index);
// Retrieve a parameter from the command
CommandParameter* CommandGetParameter(Command* cmd, uint32_t index);
// Get the number of parameters sotred in the command
uint32_t CommandGetParameterCount(Command* cmd);
// Get the CommandDesc from the command
CommandDesc* CommandGetDescription(Command* cmd);
// Check if a flag is set inside the command
uint8_t CommandIsFlagSet(Command* cmd, uint32_t flag);
// Clone the command
Command* CommandClone(Command* cmd);





//============================================
//              CommandManager
//============================================

CommandManager* CommandManagerCreate();
void CommandManagerDestroy(CommandManager* cmdMgr);
uint32_t CommandManagerGetCommandDescCount(CommandManager* cmdMgr);
uint32_t CommandManagerGetCommandQueueCount(CommandManager* cmdMgr);
CommandDesc* CommandManagerGetCommandDesc(CommandManager* cmdMgr, uint32_t index);
CommandDesc* CommandManagerGetCommandDescById(CommandManager* cmdMgr, uint16_t cmdId);
// void CommandManagerAssignDesc(CommandManager* cmdMgrTarget, CommandManager* cmdMgrSource);   // Is this needed?
uint8_t CommandManagerAddCommandDesc(CommandManager* cmdMgr, CommandDesc* cmdDesc);
uint8_t CommandManagerPostCommand(CommandManager* cmdMgr, Command* cmd);
Command* CommandManagerGetCommand(CommandManager* cmdMgr);
Command* CommandManagerPeekCommand(CommandManager* cmdMgr);
uint8_t CommandManagerParseMessage(CommandManager* cmdMgr, Command* cmd, char* sErrorMsg, int errMsgMaxLen, const char* msg);
void CommandManagerAddAlias(CommandManager* cmdMgr, const char* cmd, const char* alias);


#define BEGIN_CMD_DESC  { CommandDesc* CD; CommandManager* CM = _pCommandManager;
#define END_CMD_DESC    }
#define C(_a,_b,_c,_d)  { CD = CommandDescCreate(_a, _b, _c, _d); CommandManagerAddCommandDesc(CD); }
#define P(_a, _b)       { CommandDescAddParamDesc(CD, CommandParameterDescCreate(_a, _b)); }
#define CA(_a, _b)      { CommandManagerAddAlias(CM, _a, _b); }

// Add parameter condition?

#endif
