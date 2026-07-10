#pragma once

#include <exception>

namespace Arktos
{
    class StateException : public std::exception
    {
    public:
        StateException(const char* msg) : message(msg){}

        const char* what() const noexcept override
        {
            return message;
        }

        static void Check(const bool condition, const char* msg)
        {
            if (!condition)
            {
                throw StateException(msg);
            }
        }

    private:
        const char *message;
    };
}