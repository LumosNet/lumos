#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xor.h"
#include "lenet.h"
#include "mnist.h"

#define VERSION "0.4-a"

void analysis_argv(int argc, char **argv)
{
    if (argc <= 1)
        return;
    if (0 == strcmp(argv[1], "--version"))
    {
        char version[] = VERSION;
        fprintf(stderr, "Lumos version: v%s\n", version);
        fprintf(stderr, "This is free software; see the source for copying conditions.  There is NO\n");
        fprintf(stderr, "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
    }
    else if (0 == strcmp(argv[1], "--demo"))
    {
        if (0 == strcmp(argv[2], "xor"))
        {
            xor();
        }
        else if (0 == strcmp(argv[2], "mnist"))
        {
            full_connect_mnist();
        }
        else if (0 == strcmp(argv[2], "lenet5"))
        {
            lenet();
        }
    }
    else if (0 == strcmp(argv[1], "--help") || 0 == strcmp(argv[1], "-h"))
    {
        fprintf(stderr, "Usage commands:\n");
        fprintf(stderr, "    --version : To get version\n");
        fprintf(stderr, "    --demo : To run the demos\n");
        fprintf(stderr, "        xor : Run the basic net\n");
        fprintf(stderr, "        mnist : Run the mnist demo created by all connect layer\n");
        fprintf(stderr, "        lenet5 : Run the classic convolutional net LeNet5\n");
        fprintf(stderr, "    Run demo command such as : lumos --demo xor\n");
        fprintf(stderr, "Thank you for using Lumos deeplearning framework.\n");
    }
    else
    {
        fprintf(stderr, "Lumos: \e[0;31merror\e[0m: unrecognized command line option '%s'\n", argv[1]);
        fprintf(stderr, "compilation terminated.\n");
        fprintf(stderr, "use --help or -h to get available commands.\n");
    }
}

int main(int argc, char **argv)
{
    analysis_argv(argc, argv);
    return 0;
}
