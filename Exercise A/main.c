#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, char * argv[])
{
    int fd1, fd2;

    if(argc != 3) {
        printf( "insufficent files");
        return 1;
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        printf("open failed");
        return 1;
    }

    fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        printf("open failed");
    	return 1;
    }
    
    char buffer1[1], buffer2[1];
    int char1,char2;
    int flag = 1;



    while ( flag ) 
    {
        char1 = read(fd1,buffer1,1) ;
        char2 = read(fd2,buffer2,1) ;

        printf("buffer 1 :");
        printf("%c\n" , buffer1[0]);  
        
        if ( char1 != char2 || buffer1[0] != buffer2[0])
        {   
            printf("Files are not equal.\n");      
            close(fd1);
            close(fd2);
            return 1;
        }

        flag = (char1 == char2) && char1 != 0 && char2 != 0;
        printf("buffer 2 :");
        printf("%c\n" , buffer2[0]);  
    }    
    
    printf("%d\n" , char1);  
    printf("%d\n" , char2);  

    printf("Files are equal.\n");

    close(fd1);
    close(fd2);

    return 2;
}
