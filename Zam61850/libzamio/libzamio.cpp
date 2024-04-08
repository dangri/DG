#include "libzamio.h"

/*
	To test the library, include "libzamio.h" from an application project
	and call libzamioTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

int libzamioTest()
{
	return ++s_Test;
}