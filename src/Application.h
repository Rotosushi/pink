#ifndef APPLICATION_H
#define APPLICATION_H

#include "TypeJudgement.h"
struct Ast;
struct Environment;

typedef struct Application
{
  struct Ast* lhs;
  struct Ast* rhs;
} Application;

Application* CreateApplication(struct Ast* left, struct Ast* right);

void DestroyApplication(Application* app);

Application* CloneApplication(Application* app);

char* ToStringApplication(Application* app);

TypeJudgement GetypeApplication(Application* app, struct Environment* env);

#endif // !APPLICATION_H
