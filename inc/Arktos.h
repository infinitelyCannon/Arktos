#pragma once

#include "Version.h"

namespace Arktos
{
    class Engine
    {
    public:
        void Run();
    };

    void PrintVersion();
    /* Windows is being a pain in the butt about returning a u8 string
     * So this function is in timeout until I figure this out
     * Might need to jump to c++20 but do I even *need* this function?
     */
    //static std::string GetVersion() {return {ARKTOS_VERSION_STR};}
}