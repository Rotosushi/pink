#include "StringInterner.hpp"


namespace pink {
    InternedString StringInterner::Intern(const char* str)
    {
        auto elem = set.try_emplace(str);
        InternedString result = (elem.first->getKey()).data();
        return result;
    }

    InternedString StringInterner::Intern(std::string& str)
    {
        auto elem = set.try_emplace(str);
        InternedString result = (elem.first->getKey()).data();
        return result;
    }

}
