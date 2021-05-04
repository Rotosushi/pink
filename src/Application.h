#ifndef APPLICATION_H
#define APPLICATION_H


#include "Judgement.h"
#include "Location.h"
struct Ast;
struct Environment;

typedef struct Application
{
  Location    loc;
  int         num_args;
  struct Ast* lhs;
  struct Ast* rhs;
} Application;

void InitializeApplication(Application* app, struct Ast* lhs, struct Ast* rhs, int num_args, Location* loc);

void DestroyApplication(Application* app);

void CloneApplication(Application* destination, Application* source);

char* ToStringApplication(Application* app);

// if we want to connect location data with error messages
// we need to pass in Ast* to access the location data
// and the Application term data. (we can assume we are
// passed an application node however, the code will
// not dispatch here were it not so.)
Judgement GetypeApplication(Application* app, struct Environment* env);

Judgement EvaluateApplication(Application* app, struct Environment* env);

bool AppearsFreeApplication(Application* app, InternedString id);

void RenameBindingApplication(Application* app, InternedString target, InternedString replacement);

void SubstituteApplication(Application* app, struct Ast** target, InternedString id, struct Ast* value, struct Environment* env);
#endif // !APPLICATION_H
