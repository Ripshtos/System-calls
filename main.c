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

int compareFiles(char *file1, char *file2)
{
    int fd1, fd2;

    fd1 = open(file1, O_RDONLY);
    if (fd1 < 0)
    {
        printf("open failed");
        return 1;
    }

    fd2 = open(file2, O_RDONLY);
    if (fd2 < 0)
    {
        printf("open failed");
        return 1;
    }

    char buffer1[1], buffer2[1];
    int char1, char2;
    int flag = 1;

    while (flag)
    {
        char1 = read(fd1, buffer1, 1);
        char2 = read(fd2, buffer2, 1);

        if (char1 != char2 || buffer1[0] != buffer2[0])
        {
            printf("Files are not equal.\n");
            close(fd1);
            close(fd2);
            return 1;
        }

        flag = (char1 == char2) && char1 != 0 && char2 != 0;
    }

    printf("Files are equal.\n");

    close(fd1);
    close(fd2);

    return 2;
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

    int resultsFile = open("results.csv", O_WRONLY);

    if (resultsFile == NULL)
    {
        perror("fopen");
        closedir(dir);
        return 1;
    }

    char *title = "Name, Grade\n";
    int first = write(resultsFile, title, strlen(title));

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

        printf("%s \n", entry->d_name);
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

        pid = wait(NULL);

        int *inputFd = open(keyboardInputFile, O_RDONLY); //!!!!

        if (inputFd == -1)
        {
            printf("error opening file");
            return;
        }

        char *num1 = readLine(&inputFd);
        char *num2 = readLine(&inputFd);
        close(inputFd);

        int pid2 = fork(); // compile student project

        if (pid2 == 0)
        {
            int fd = open("studentoutput.txt", O_WRONLY); // writes to student output file
            if (fd == -1)
            {
                printf("Error opening studentoutput.txt file");
                exit(1);
            }

            close(1);
            dup(fd);

            int execute = execlp(output, output, num1, num2, (char *)NULL);
            if (execute == -1)
            {
                printf("error compiling student file , no access");
                exit(1);
            }
        }

        pid2 = wait(NULL);

      

        char *nameGrade = entry->d_name;

        if (compareFiles("studentoutput.txt", "expectedOutput.txt") == 2) // condition for student to get a 100
        {
            printf("in! ");
            // Open the file for writing
            nameGrade = entry->d_name;
            strcat(nameGrade, ",100\n");
            int write100 = write(resultsFile, nameGrade, strlen(nameGrade));
        }
        else
        {
            nameGrade = entry->d_name;
            strcat(nameGrade, ",0\n");
            printf("%s", nameGrade);
            int write0 = write(resultsFile, nameGrade, strlen(nameGrade));
        }

        printf("\n");
    }

    close(resultsFile);
    closedir(dir);
    close(fd1);

    printf("finished \n");
    exit(1);
    return 1;
}
