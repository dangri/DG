#include "libzamgo.h"

/*
	To test the library, include "libzamgo.h" from an application project
	and call libzamgoTest().
	
	Do not forget to add the library to Project Dependencies in Visual Studio.
*/

static int s_Test = 0;

int libzamgoTest()
{
	return ++s_Test;
}