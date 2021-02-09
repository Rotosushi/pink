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

void InitializeApplication(Application* app, struct Ast* lhs, struct Ast* rhs);

void DestroyApplication(Application* app);

void CloneApplication(Application* app, Application* app);

char* ToStringApplication(Application* app);

TypeJudgement GetypeApplication(Application* app, struct Environment* env);

#endif // !APPLICATION_H
