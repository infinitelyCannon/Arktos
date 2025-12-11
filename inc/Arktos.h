#pragma once

#include "Version.h"
#include <string>

namespace Arktos
{
    class Engine
    {
    public:
        void Run();
    };

    void PrintVersion();
    static std::string GetVersion() {return {ARKTOS_VERSION_STR};}
}