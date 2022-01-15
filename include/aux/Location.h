#pragma once
#include <cstddef> // size_t

namespace pink {
    class Location {
    public:
        size_t firstLine;
        size_t firstColumn;
        size_t lastLine;
        size_t lastColumn;

        Location();
        Location(const Location& other);
        Location(size_t fl, size_t fc, size_t ll, size_t lc);

        bool operator==(const Location& other);
    };

}
