#ifndef INTEGER_H
#define INTEGER_H

typedef struct Integer
{
  int value;
} Integer;

void InitializeInteger(Integer* integer, int value);

void DestroyInteger(Integer* integer);

void CloneInteger(Integer* destination, Integer* source);

char* ToStringInteger(Integer* integer);

#endif // !INTEGER_H
