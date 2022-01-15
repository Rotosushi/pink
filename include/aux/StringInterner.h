#pragma once
#include <string>
#include <utility> // pair

#include <llvm/ADT/StringSet.h>

namespace pink {
    typedef const char* InternedString;

    class StringInterner {
    private:
        llvm::StringSet<> set;

    public:
        StringInterner();
        StringInterner(const StringInterner& other);
        // #TODO: refactor Intern to return a pair<InternedString, bool>
        // which reflects the return type of llvm::StringSet<>::try_emplace
        // which returns a bool which represents if the table performed
        // and insertion (true) or not (false). This peice of information
        // is useful when considering refactoring the language to include
        // a precompilation 'macro-expansion' step within the compiler.
        InternedString Intern(const char* str);
        InternedString Intern(std::string& str);
    };


}
