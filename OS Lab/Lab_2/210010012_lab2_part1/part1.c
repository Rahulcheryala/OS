#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void print_string(int position, char *str)
{
    if (position < strlen(str))
    {
        printf("Character : %c, Process ID : %d\n", str[position], getpid());
        int sleep_time = (rand() % 4) + 1;
        sleep(sleep_time);
        pid_t pid = fork();
        // recursive child call
        if (pid == 0)
        {
            // child process
            print_string(position + 1, str);
        }
        else
        {
            // parent process
            wait(NULL);
        }
    }
}
int main(int argc, char *argv[])
{
    char str[] = "Hello World\0";
    int position = 0;
    print_string(position, str);
    return 0;
}