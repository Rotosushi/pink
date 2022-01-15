
#include "aux/Location.h"


namespace pink {
    Location::Location()
        :   firstLine(0), firstColumn(0), lastLine(0), lastColumn(0)
    {

    }

    Location::Location(const Location& other)
        :   firstLine(other.firstLine), firstColumn(other.firstColumn),
            lastLine(other.lastLine), lastColumn(other.lastColumn)
    {

    }

    Location::Location(size_t fl, size_t fc, size_t ll, size_t lc)
        : firstLine(fl), firstColumn(fc), lastLine(ll), lastColumn(lc)
    {

    }

    bool Location::operator==(const Location& other)
    {
        bool result;
        result = (firstLine == other.firstLine)
                && (firstColumn == other.firstColumn)
                && (lastLine == other.lastLine)
                && (lastColumn == other.lastColumn);
        return result;
    }
}
