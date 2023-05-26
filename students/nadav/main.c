#include <stdio.h>

int main(int argc, char * argv[])
{
	if(argc != 3 )
	{
		printf("invalid input");
		return;
	}
	printf("The output is : %d \n" , atoi(argv[1]) + atoi(argv[2])  );
	return 1;
}
