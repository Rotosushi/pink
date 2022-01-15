#pragma once
#include <string>
#include <iostream>

#include "aux/Location.h"

namespace pink {
    class Error {
    public:
        enum class Kind {
            Syntax,
            Type,
            Semantic,
        };

    private:
        Kind        kind;
        std::string dsc;
        Location    loc;

    public:
        Error(const Error& other);
        Error(Kind k, const char* d, Location l);
        Error(Kind k, std::string& d, Location l);

        Error& operator=(const Error& other);

        std::string ToString(const char* txt);
        std::string ToString(std::string& txt);

        std::ostream& Print(std::ostream& out, std::string& txt);
        std::ostream& Print(std::ostream& out, const char* txt);
    };

    void FatalError(const char* dsc, const char* file, size_t line);
}
