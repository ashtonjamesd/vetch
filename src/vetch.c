#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vetch.h"

static Array *newArray(int elementSize) {
    Array *arr = malloc(sizeof(Array));
    arr->capacity = 1;
    arr->count = 0;
    arr->elementSize = elementSize;
    arr->entries = malloc(sizeof(void *) * arr->capacity);

    return arr;
}

static void arrayAdd(Array *arr, void *element) {
    if (arr->count >= arr->capacity) {
        arr->capacity *= 2;
        arr->entries = realloc(arr->entries, sizeof(void *) * arr->capacity);
    }

    arr->entries[arr->count++] = element;
}

VetchArgParser *vetchInit(void) {
    VetchArgParser *vetch = malloc(sizeof(VetchArgParser));
    if (!vetch) {
        return NULL;
    }

    vetch->commands = newArray(sizeof(ArgNode *));
    vetch->globalFlags = newArray(sizeof(ArgNode *));

    vetch->unknownFlagMessage = NULL;
    vetch->unknownCommandMessage = NULL;

    return vetch;
}

void vetchDeinit(VetchArgParser *vetch) {
    for (int i = 0; i < vetch->globalFlags->count; i++) {
        ArgNode *flag = (ArgNode *)vetch->globalFlags->entries[i];
        
        free(flag->name);
        free(flag);
    }

    for (int i = 0; i < vetch->commands->count; i++) {
        ArgNode *cmd = (ArgNode *)vetch->commands->entries[i];
        
        free(cmd->name);
        free(cmd);
    }

    free(vetch->commands);
    free(vetch->globalFlags);

    if (vetch->unknownFlagMessage) {
        free(vetch->unknownFlagMessage);
    }
    
    if (vetch->unknownCommandMessage) {
        free(vetch->unknownCommandMessage);
    }

    free(vetch);
}

static ArgNode *newFlag(char *name) {
    ArgNode *flag = malloc(sizeof(ArgNode));
    if (!flag) {
        return NULL;
    }

    flag->name = strdup(name);
    flag->type = ARG_TYPE_FLAG;
    flag->as.flag.aliases = newArray(sizeof(char *));

    return flag;
}


void setGlobalFlag(VetchArgParser *vetch, char *name) {
    ArgNode *flag = newFlag(name);
    arrayAdd(vetch->globalFlags, flag);
}

void setFlag(ArgNode *node, char *name) {
    ArgNode *flag = newFlag(name);
    arrayAdd(node->flags, flag);
}

void setFlags(ArgNode *node, char *flags) {
    char *flagsCopy = strdup(flags);
    if (!flagsCopy) return;

    char *token = strtok(flagsCopy, ",");

    ArgNode *flag = newFlag(token);

    while (token) {
        while (*token == ' ') token++;

        arrayAdd(flag->as.flag.aliases, strdup(token));

        token = strtok(NULL, ",");
    }

    arrayAdd(node->flags, flag);

    free(flagsCopy);
}

void setHelp(VetchArgParser *vetch, char *message) {
    vetch->helpString = strdup(message);

    setGlobalFlag(vetch, "--help");
    setGlobalFlag(vetch, "-h");
}

void setVersion(VetchArgParser *vetch, char *message) {
    vetch->versionString = strdup(message);

    setGlobalFlag(vetch, "--version");
    setGlobalFlag(vetch, "-v");
}

ArgNode *setCommand(VetchArgParser *vetch, char *name) {
    ArgNode *node = malloc(sizeof(ArgNode));
    if (!node) {
        return NULL;
    }

    node->name = strdup(name);
    node->parent = NULL;
    node->isFound = false;
    node->flags = newArray(sizeof(ArgNode));
    node->subcommands = newArray(sizeof(ArgNode));
    
    arrayAdd(vetch->commands, node);

    return node;
}

ArgNode *setSubCommand(ArgNode *parent, char *name) {
    ArgNode *node = malloc(sizeof(ArgNode));
    if (!node) {
        return NULL;
    }

    node->name = strdup(name);
    node->parent = parent;
    node->isFound = false;
    node->flags = newArray(sizeof(ArgNode));
    node->subcommands = newArray(sizeof(ArgNode));

    arrayAdd(parent->subcommands, node);

    return node;
}

void setUnknownFlagMessage(VetchArgParser *vetch, char *message) {
    vetch->unknownFlagMessage = strdup(message);
}

void setUnknownCommandMessage(VetchArgParser *vetch, char *message) {
    vetch->unknownCommandMessage = strdup(message);
}

static void printCommand(ArgNode *command) {
    // if (command->parent) {
    //     printf("%s ", command->parent->name);
    // }

    ArgNode *current = command;
    ArgNode *parentChain[32];
    int depth = 0;

    while (current) {
        parentChain[depth++] = current;
        current = current->parent;
    }

    for (int i = depth - 1; i >= 0; i--) {
        printf("%s ", parentChain[i]->name);
    }

    for (int i = 0; i < command->flags->count; i++) {
        ArgNode *node = (ArgNode *)command->flags->entries[i];

        if (node->type == ARG_TYPE_FLAG) {
            printf(" [");
            for (int i = 0; i < node->as.flag.aliases->count; i++) {
                printf((char *)node->as.flag.aliases->entries[i]);
                if (i != node->as.flag.aliases->count - 1) printf(" | ");
            }

            if (node->as.flag.aliases->count == 0) {
                printf(node->name);
            }
            printf("] ");
        }
    }

    printf("\n  ");

    for (int i = 0; i < command->subcommands->count; i++) {
        ArgNode *sub = (ArgNode *)command->subcommands->entries[i];
        printCommand(sub);
    }
}

void vetchPrint(VetchArgParser *vetch) {
    printf("\n\nFLAGS:\n  ");
    for (int i = 0; i < vetch->globalFlags->count; i++) {
        printf("[%s] ", ((ArgNode *)vetch->globalFlags->entries[i])->name);
    }

    printf("\n\nCOMMANDS:\n  ");
    for (int i = 0; i < vetch->commands->count; i++) {
        ArgNode *node = (ArgNode *)vetch->commands->entries[i];

        printCommand(node);
    }
}

static ArgNode *findGlobalFlag(VetchArgParser *vetch, char *flagName) {
    for (int j = 0; j < vetch->globalFlags->count; j++) {
        ArgNode *flag = (ArgNode *)vetch->globalFlags->entries[j];

        if (strcmp(flag->name, flagName) == 0) {
            return flag;
        }
    }

    return NULL;
}

static ArgNode *findCommand(VetchArgParser *vetch, char *commandName) {
    for (int j = 0; j < vetch->commands->count; j++) {
        ArgNode *command = (ArgNode *)vetch->commands->entries[j];

        if (strcmp(command->name, commandName) == 0) {
            return command;
        } else {
            for (int i = 0; i < command->subcommands->count; i++) {
                ArgNode *subcommand = (ArgNode *)command->subcommands->entries[i];
                
                if (strcmp(subcommand->name, commandName) == 0) {
                    return subcommand;
                }
            }
        }
    }

    return NULL;
}

bool hasSubFlag(ArgNode *node, char *flag) {
    for (int i = 0; i < node->flags->count; i++) {
        ArgNode *subflag = (ArgNode*)node->flags->entries[i];

        if (strcmp(subflag->name, flag) == 0) {
            return true;
        }
    }
    
    return false;
}

bool hasFlag(VetchArgParser *vetch, char *flag) {
    ArgNode *arg = findGlobalFlag(vetch, flag);
    if (!arg) return false;

    return arg->isFound;
}

bool hasCommand(VetchArgParser *vetch, char *flag) {
    ArgNode *arg = findCommand(vetch, flag);
    if (!arg) return false;

    return arg->isFound;
}

bool hasSubCommand(ArgNode *node, char *command) {
    for (int i = 0; i < node->subcommands->count; i++) {
        ArgNode *subcommand = ((ArgNode *)node->subcommands->entries[i]);

        if (strcmp(subcommand->name, command) == 0) {
            return subcommand->isFound;
        }
    }

    return false;
}

void vetchParse(VetchArgParser *vetch, int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        ArgNode *flag = findGlobalFlag(vetch, argv[i]);
        ArgNode *command = findCommand(vetch, argv[i]);

        // flags such as -A for 'add' do not work
        // they need to be parsed here

        if (command) {
            for (int j = 0; j < command->subcommands->count; j++) {
                ArgNode *subcommand = (ArgNode *)command->subcommands->entries[j];

                if (strcmp(subcommand->name, argv[i]) == 0) {
                    subcommand->isFound = true;
                }
            }
        }

        if (!flag && !command) {
            if (vetch->unknownFlagMessage) {
                printf("%s: '%s'\n", vetch->unknownFlagMessage, argv[i]);
            } else if (vetch->unknownCommandMessage) {
                printf("%s: '%s'\n", vetch->unknownCommandMessage, argv[i]);
            }
            break;
        }

        if (flag) {
            flag->isFound = true;

            if (strcmp(flag->name, "-h") == 0 || strcmp(flag->name, "--help") == 0) {
                printf(vetch->helpString);
                return;
            } else if (strcmp(flag->name, "-v") == 0 || strcmp(flag->name, "--version") == 0) {
                printf(vetch->versionString);
                return;
            }
        }

        if (command) {
            command->isFound = true;
        }
    }
}