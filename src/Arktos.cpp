#include <iostream>
#include "Arktos.h"

#if WIN32
#include "win/UTF8CodePage.h"
#endif

void Arktos::PrintVersion()
{
#if WIN32
	UTF8CodePage cp;
#endif
	std::cout << ARKTOS_VERSION_STR << std::endl;
}