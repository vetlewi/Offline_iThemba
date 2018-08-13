

#include <stdio.h>
#include <fnmatch.h>
#include <stdlib.h>


int main()
{
	
	char buffer[1024];

	sprintf(buffer, "/Volumes/PR282/PR282/R78_21");

	int res = fnmatch("/Volumes/PR282/PR282/R78_*", buffer, 0);

	printf("Result %i\n", res);

	return 0;
}