#ifndef INTEGER_H
#define INTEGER_H

typedef struct Integer
{
  int value;
} Integer;

void DestroyInteger(Integer* i);

Integer* CloneInteger(Integer* i);

char* ToStringInteger(Integer* i);

#endif // !INTEGER_H
