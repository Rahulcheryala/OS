#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int op = atoi(argv[argc - 1]); // op is the last argument
    op = op / 2;
    printf("Half: Current process id: %d, Current result: %d\n", getpid(), op);
    char *arglist[argc]; // array to store command line arguments
    for (int i = 0; i < argc - 2; i++)
    {
        arglist[i] = argv[i + 1];
    }
    arglist[argc - 2] = malloc(sizeof(char));
    sprintf(arglist[argc - 2], "%d", op);
    arglist[argc - 1] = NULL;
    execvp(arglist[0], arglist);
    return 0;
}