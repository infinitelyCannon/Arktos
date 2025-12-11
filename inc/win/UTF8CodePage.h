#pragma once

/*
* This class exists because Windows loves to be difficult when it comes to UTF8.
* Some characters in the "version text" require it and this is a workaround 
* to switch to code page to UTF8 on Windows Terminal (which can render it).
*/
class UTF8CodePage
{
public:
	UTF8CodePage();
	~UTF8CodePage();

private:
	unsigned int mOldCodePage;
};