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

    if (!hasCommand(v, "git")) {
        printf("all commands start with 'git'!");
    }

    if (hasSubCommand(git, "add")) {
        printf("add.."); // do add stuff
    }
    else if (hasSubCommand(git, "commit")) {
        printf("commit.."); // do commit stuff
    }

    if (hasSubFlag(add, "-A")) {
        printf("adding -A.."); // do add -A stuff
    }

    vetchPrint(v);
    vetchDeinit(v);

    return 0;
}