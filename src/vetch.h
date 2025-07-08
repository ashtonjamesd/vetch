#ifndef vetch_h
#define vetch_h

#include <stdbool.h>

typedef struct {
    int    count;
    int    capacity;
    int    elementSize;
    void **entries;
} Array;

typedef enum {
    ARG_TYPE_FLAG,
    ARG_TYPE_COMMAND,
} ArgumentType;

typedef struct {
    char *name;
    
    // stores aliases to the main flag
    // for instance, -h is an alias to --help
    Array *aliases;
} ArgumentFlag;

typedef struct {
    int dummy;
} ArgumentCommand;

typedef struct ArgNode {
    union {
        ArgumentFlag flag;
        ArgumentCommand command;
    } as;
    ArgumentType type;

    char  *name;

    Array *flags;
    Array *subcommands;

    struct ArgNode *parent;
    bool   isFound;
} ArgNode;

typedef struct {
    Array *commands;
    Array *globalFlags;

    char  *helpString;
    char  *versionString;
    char  *unknownFlagMessage;
    char  *unknownCommandMessage;
} VetchArgParser;


// Allocates and initializes a new vetch struct 
VetchArgParser* vetchInit(void);


// Frees all memory used by a vetch struct
void vetchDeinit(VetchArgParser *vetch);


// Sets help message for the parser
void setHelp(VetchArgParser *vetch, char *message);


// Registers a new flag
void setFlag(ArgNode *node, char *name);


// Regsiters multiple flags with a delimeter
void setFlags(ArgNode *node, char *flags);


// Registers global flags
void setGlobalFlag(VetchArgParser *vetch, char *name);


// Sets help message for the parser
void setVersion(VetchArgParser *vetch, char *message);


// Registers a new command
ArgNode *setCommand(VetchArgParser *vetch, char *command);


// Registers a new sub command
ArgNode *setSubCommand(ArgNode *node, char *command);


// Sets the message to display when an unknown flag or argument is found
void setUnknownFlagMessage(VetchArgParser *vetch, char *message);


// Sets the message to display when an unknown flag or argument is found
void setUnknownCommandMessage(VetchArgParser *vetch, char *message);


// Returns true if the flag was found
bool hasFlag(VetchArgParser *vetch, char *flag);


// Returns true if the subflag was found
bool hasSubFlag(ArgNode *node, char *flag);


// Returns true if the command was found
bool hasCommand(VetchArgParser *vetch, char *flag);


// Returns true if the subcommand was found
bool hasSubCommand(ArgNode *vetch, char *command);


// Parses the arguments
void vetchParse(VetchArgParser *vetch, int argc, char **argv);


// Prints current state of the vetch struct
void vetchPrint(VetchArgParser *vetch);

#endif