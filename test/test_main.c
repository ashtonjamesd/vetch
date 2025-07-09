#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/vetch.h"

static int tests_run = 0;
static int tests_failed = 0;

#define ASSERT(cond, msg) do { \
    tests_run++;               \
    if (!(cond)) {             \
        fprintf(stderr, "Assertion failed: %s\n", msg); \
        tests_failed++;        \
    }                         \
} while (0)

void test_init_and_deinit() {
    VetchArgParser *v = vetchInit();
    ASSERT(v != NULL, "Failed to initialize vetch parser");
    vetchDeinit(v);
}

void test_add_commands_and_subcommands() {
    VetchArgParser *v = vetchInit();
    ArgNode *cmd = setCommand(v, "cmd");
    ASSERT(cmd != NULL, "Failed to add command");
    ArgNode *sub = setSubCommand(cmd, "sub");
    ASSERT(sub != NULL, "Failed to add subcommand");

    ASSERT(strcmp(cmd->name, "cmd") == 0, "Command name incorrect");
    ASSERT(strcmp(sub->name, "sub") == 0, "Subcommand name incorrect");
    ASSERT(sub->parent == cmd, "Subcommand parent incorrect");

    vetchDeinit(v);
}

void test_add_flags_and_aliases() {
    VetchArgParser *v = vetchInit();
    ArgNode *cmd = setCommand(v, "cmd");
    setFlags(cmd, "-a, --all");
    setFlag(cmd, "--verbose");

    ASSERT(cmd->flags->count == 2, "Flags count incorrect");

    ArgNode *flagA = (ArgNode *)cmd->flags->entries[0];
    ASSERT(flagA->as.flag.aliases->count == 2, "Aliases count incorrect");
    ASSERT(strcmp(flagA->as.flag.aliases->entries[0], "-a") == 0, "Alias -a missing");
    ASSERT(strcmp(flagA->as.flag.aliases->entries[1], "--all") == 0, "Alias --all missing");

    ArgNode *flagVerbose = (ArgNode *)cmd->flags->entries[1];
    ASSERT(strcmp(flagVerbose->name, "--verbose") == 0, "Flag --verbose missing");

    vetchDeinit(v);
}

void test_parse_basic_command() {
    VetchArgParser *v = vetchInit();
    ArgNode *cmd = setCommand(v, "cmd");

    int argc = 2;
    char *argv[] = {"program", "cmd"};
    vetchParse(v, argc, argv);

    ASSERT(hasCommand(v, "cmd"), "Command 'cmd' not detected");

    vetchDeinit(v);
}

void test_parse_subcommand_and_flags() {
    VetchArgParser *v = vetchInit();
    ArgNode *cmd = setCommand(v, "git");
    ArgNode *add = setSubCommand(cmd, "add");
    setFlags(add, "--verbose, -v");
    setFlag(add, "-A");

    int argc1 = 4;
    char *argv1[] = {"program", "git", "add", "-A"};
    vetchParse(v, argc1, argv1);

    ASSERT(hasCommand(v, "git"), "Command 'git' missing");
    ASSERT(hasSubCommand(cmd, "add"), "Subcommand 'add' missing");
    ASSERT(hasSubFlag(add, "-A"), "Flag '-A' missing");
    ASSERT(!hasSubFlag(add, "-v"), "Flag '-v' should not be found");

    int argc2 = 6;
    char *argv2[] = {"program", "git", "add", "-v", "--verbose", "--dry-run"};
    vetchParse(v, argc2, argv2);

    ASSERT(hasSubFlag(add, "-v"), "Flag '-v' missing after parse");
    ASSERT(hasSubFlag(add, "--verbose"), "Flag '--verbose' missing after parse");

    vetchDeinit(v);
}

void test_help_and_version_flags() {
    VetchArgParser *v = vetchInit();
    setHelp(v, "Help message");
    setVersion(v, "Version 1.0");

    int argc1 = 2;
    char *argv1[] = {"program", "-h"};
    vetchParse(v, argc1, argv1);
    ASSERT(hasFlag(v, "-h"), "Help flag '-h' missing");

    int argc2 = 2;
    char *argv2[] = {"program", "--version"};
    vetchParse(v, argc2, argv2);
    ASSERT(hasFlag(v, "--version"), "Version flag '--version' missing");

    vetchDeinit(v);
}

void run_all_tests() {
    test_init_and_deinit();
    test_add_commands_and_subcommands();
    test_add_flags_and_aliases();
    test_parse_basic_command();
    test_parse_subcommand_and_flags();
    test_help_and_version_flags();

    printf("\n\nAssertions run: %d\n", tests_run);
    if (tests_failed) {
        printf("Assertions failed: %d\n", tests_failed);
        exit(1);
    } else {
        printf("All tests passed!\n");
        exit(0);
    }
}

int main() {
    run_all_tests();
    return 0;
}
