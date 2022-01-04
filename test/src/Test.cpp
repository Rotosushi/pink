
#include "Test.hpp"

#include "TestError.hpp"
#include "TestStringInterner.hpp"

size_t Test(std::ostream& out, size_t flags)
{
    size_t result = 0;

    if ((flags & TEST_ERROR) > 0)
    {
        if (TestError(out))
            result |= TEST_ERROR;
    }

    if ((flags & TEST_STRING_INTERNER) > 0)
    {
        if (TestStringInterner(out))
            result |= TEST_STRING_INTERNER;
    }

    return result;
}
