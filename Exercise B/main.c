#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

char *readLine(int *fd)
{
    int bytes_read;
    char *line = (char *)malloc(sizeof(char));
    int idx = 0;
    char c;
    while ((bytes_read = read(*fd, &c, sizeof(char))) > 0 && c != '\n')
    {
        line = (char *)realloc(line, (idx + 1) * sizeof(char) + sizeof(char));
        line[idx++] = c;
    }

    line[idx] = '\0';
    return line;
}

int main(int argc, char *argv[])
{

    int fd1;

    if (argc != 2)
    {
        printf("insufficent files");
        return 1;
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0)
    {
        printf("open failed");
        return 1;
    }

    char fileB[100], inputB[100], outputB[100];
    int file, input, output;

    int counter = 0;

    char *studentPath = readLine(&fd1);
    char *keyboardInputFile = readLine(&fd1);
    char *expectedOutputFile = readLine(&fd1);

    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(studentPath);

    while ((entry = readdir(dir)) != NULL)
    {

        // Skip directories "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char output[256];
        strcpy(output, "./students/");
        strcat(output, entry->d_name);
        strcat(output, "/main.exe");

        char input[256];
        strcpy(input, "./students/");
        strcat(input, entry->d_name);
        strcat(input, "/main.c");

        printf("%s \n", input);
        printf("%s \n", output);

        int pid = fork();
        if (pid == 0)
        {
            int execute = execlp("gcc", "gcc", input, "-o", output, NULL);
            if (execute == -1)
            {
                printf("error compiling file");
                exit(1);
            }
        }

        int *inputFd = open(keyboardInputFile, O_RDONLY); //!!!!

        if (inputFd == -1)
        {
            printf("error opening file");
            return;
        }

        char *num1 = readLine(&inputFd);
        char *num2 = readLine(&inputFd);
        close(inputFd);

        char scriptResult[1000];

        
        int pid2 = fork(); // compile student project

        if (pid2 == 0)
        {
            int execute = execlp(output, output, num1, num2, (char *)NULL);
            if (execute == -1)
            {
                printf("error compiling student file");
                exit(1);
            }

            
        }


        pid2 = wait(NULL);

        int *outputFd = open(expectedOutputFile, O_RDONLY); //!!!!
        if (outputFd == -1)
        {
            printf("error opening output file");
            exit(1);
        }

        char *resultNum = readLine(&outputFd);

        close(outputFd);

        printf("\n");
    }
    closedir(dir);
    close(fd1);

    printf("finished \n");
    exit(1);
    return 1;
}
