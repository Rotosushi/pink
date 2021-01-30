#ifndef APPLICATION_H
#define APPLICATION_H

struct Ast;

typedef struct Application
{
  struct Ast* lhs;
  struct Ast* rhs;
} Application;

Application* CreateApplication(struct Ast* left, struct Ast* right);

void DestroyApplication(Application* app);

Application* CloneApplication(Application* app);

char* ToStringApplication(Application* app);

#endif // !APPLICATION_H
