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
#include <signal.h>
#include <time.h>

time_t startTime; // Get the current time
time_t currentTime;
time_t elapsedTime;

char * doubleToString(double number)
{
    int length = snprintf(NULL, 0, "%.15g", number);
    char* result = (char*)malloc((length + 1) * sizeof(char));
    snprintf(result, length + 1, "%.15g", number);
    return result;
}

int CheckForFile()
{
    const char *filename = "to_server.txt";
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL)
    {
        perror("opendir");
        return -1;
    }

    int fileFound = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, filename) == 0)
        {
            fileFound = 1;
            break;
        }
    }

    closedir(dir);

    return fileFound;
}

char *readLine(int *fd)
{
    ssize_t bytes_read;
    char *line = (char *)malloc(sizeof(char));
    size_t idx = 0;
    char c;
    while ((bytes_read = read(*fd, &c, sizeof(char))) > 0 && c != '\n')
    {
        line = (char *)realloc(line, (idx + 1) * sizeof(char) + sizeof(char));
        line[idx++] = c;
    }

    line[idx] = '\0';
    return line;
}

char *strCat(char *str1, char *str2)
{
    char *concat = (char *)malloc(sizeof(char));
    size_t idx = 0;

    while (*str1 != '\0')
    {
        concat = (char *)realloc(concat, (idx + 1) * sizeof(char) + sizeof(char));
        concat[idx++] = *(str1++);
    }

    while (*str2 != '\0')
    {
        concat = (char *)realloc(concat, (idx + 1) * sizeof(char) + sizeof(char));
        concat[idx++] = *(str2++);
    }

    concat[idx] = '\0';
    return concat;
}

void CreateResponse(double result, char *sendingServer)
{

    char *filename = strCat(sendingServer, ".txt");

    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    char *str1 = doubleToString(result);

    int client = write(fd, str1, strlen(str1));

    close(fd);
}

void handleSignal(int signal) // clientResult
{
    if (signal == SIGUSR1)
    {
        int forkid = fork();
        elapsedTime = 0;
        startTime = time(NULL); // restart start time 
        
        if (forkid == 0)
        {
            int *inputFd = open("to_server.txt", O_RDONLY); //!!!!

            if (inputFd == -1)
            {
                printf("error opening file");
                return;
            }
            
            char *sendingServertemp = readLine(&inputFd);
            double num1 = atof(readLine(&inputFd));
            int operation = atoi(readLine(&inputFd));
            double num2 = atof(readLine(&inputFd));

            char * sendingServer = strCat("to_client_",sendingServertemp);
            
            double result;

            close(inputFd);

            if (CheckForFile())
            {
                remove("to_server.txt");
            }
            printf("end of stage g\n");


            

            switch (operation)
            {
            case 1:
                result = num1 + num2;
                break;

            case 2:
                result = num1 - num2;
                break;

            case 3:
                result = num1 * num2;
                break;

            case 4:
                if (num2 != 0)
                {
                    result = num1 / num2;
                }
                else
                {
                    printf("ERROR FROM EX2 \n");
                    exit(0);
                }
                break;

            default:
                printf("Error:invalid operation\n");
                exit(1);
            }

            CreateResponse(result, sendingServer);

            printf("end of stage i\n");
            
            if (kill(atoi(sendingServertemp), SIGUSR2) == -1)
            {
                    printf("ERROR FROM EX2 \n");
            }
            
            exit(0);

            printf("end of stage f\n");
        }

        else
        {
            // Wait for the child process to finish
            int status;
            waitpid(forkid, &status, 0);
        
        }

    }
}



int main(int argc, char argv[])
{

    signal(SIGUSR1, handleSignal);
    if (CheckForFile())
    {
        if (remove("to_server.txt") == 0)
        {
        }
    }
    printf("end of stage c\n");

    startTime = time(NULL); // Get the start time

    while (1)
    {

        currentTime = time(NULL); // Get the current time
        elapsedTime = currentTime - startTime;

        if (elapsedTime >= 60)
        {
            printf("The server was closed because no service request was received for the last 60 seconds . \n");
            return;
        }
        
    }
}
