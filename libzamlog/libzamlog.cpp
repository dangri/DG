#include "libzamlog.h"

/*
	To test the library, include "libzamlog.h" from an application project
	and call libzamlogTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

int libzamlogTest()
{
	return ++s_Test;
}