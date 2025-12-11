#include <Windows.h>
#include "win/UTF8CodePage.h"

UTF8CodePage::UTF8CodePage() : mOldCodePage(::GetConsoleOutputCP())
{
	::SetConsoleOutputCP(CP_UTF8);
}

UTF8CodePage::~UTF8CodePage()
{
	::SetConsoleOutputCP(mOldCodePage);
}