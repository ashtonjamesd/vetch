#include <stdlib.h>
#include <stdio.h>

#include "vetch.h"


int main(int argc, char **argv) {
    VetchArgParser *v = vetchInit();
    if (!v) exit(1);

    // set help and version command outputs
    setHelp(v, "Usage: build/a git [commands]");
    setVersion(v, "v1.0.0");

    ArgNode *git = setCommand(v, "git");

    ArgNode *init = setSubCommand(git, "init");
    setFlags(init, "-q, --quiet");
    setFlag(init, "--bare");

    ArgNode *add = setSubCommand(git, "add");
    setFlags(add, "--verbose, -v");
    setFlags(add, "--dry-run, -n");
    setFlags(add, "--force, -f");
    setFlags(add, "--interactive, -i");
    setFlag(add, "-A");

    ArgNode *commit = setSubCommand(git, "commit");
    setFlags(commit, "-a, --interactive, --patch");
    setFlag(commit, "-s");
    setFlag(commit, "-v");
    setFlag(commit, "--amend");

    ArgNode *push = setSubCommand(git, "push");
    setFlags(push, "--all, --branches, --mirror, --tags");
    setFlag(push, "--atomic");
    setFlags(push, "-n, --dry-run");

    vetchParse(v, argc, argv);

    for (int i = 0; i < add->flags->count; i++) {
        ArgNode *flag = (ArgNode *)add->flags->entries[i];
        printf("%s: %d\n", flag->name, flag->isFound);
    }

    if (!hasCommand(v, "git")) {
        printf("all commands start with 'git'!\n");
    }

    if (hasSubCommand(git, "add")) {
        printf("do add stuff..\n");

        if (hasSubFlag(add, "-A")) {
            printf("do add -A stuff..\n");
        }
        if (hasSubFlag(add, "--verbose")) {
            printf("do add --verbose stuff..\n");
        }
        if (hasSubFlag(add, "--dry-run")) {
            printf("do add --dry-run stuff..\n");
        }
        if (hasSubFlag(add, "--force")) {
            printf("do add --force stuff..\n");
        }
        if (hasSubFlag(add, "--interactive")) {
            printf("do add --interactive stuff..\n");
        }
        if (hasSubFlag(add, "-i")) {
            printf("also do add -i stuff..\n");
        }
    }
    else if (hasSubCommand(git, "commit")) {
        printf("do commit stuff..\n");
    }

    vetchPrint(v);
    vetchDeinit(v);

    return 0;
}