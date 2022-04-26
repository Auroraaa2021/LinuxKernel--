#include<unistd.h>
#include<pwd.h>
#include"function.h"

bool GetCommand(char* buffer);

int main()
{
	char buffer[1000];
	getcwd(buffer, 1000);
	while(true)
		if(!GetCommand(buffer)) break;
	return 0;
}
