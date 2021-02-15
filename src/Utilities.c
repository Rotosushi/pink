#include <stdlib.h>
#include <string.h>

#include "Utilities.h"

char* dupnstr(char* str, int len)
{
    char* result = (char*)calloc(sizeof(char), len + 1);
    result = strncpy(result, str, len);
    return result;
}
