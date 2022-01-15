
#include "Test.h"

/*
    Tests don't ensure that the code is correct,
    Tests only ensure that the code is correct in every way you checked.
*/


int main(int argc, char** argv)
{
    size_t results = RunTests(std::cout, TEST_ALL);

    PrintPassedTests(std::cout, results);

    return 0;
}
