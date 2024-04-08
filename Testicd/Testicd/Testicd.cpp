
#include "test.h"

int main()
{
	int err = 0;
	// init_logging("EVV");
	auto test(make_shared<Ctest>());
	test->m_nomfichier = "IEDSE.icd";
	err = test->Demarrage();
	return err;
}
