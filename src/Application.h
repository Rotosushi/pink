#ifndef APPLICATION_H
#define APPLICATION_H


#include "TypeJudgement.h"
#include "Location.h"
struct Ast;
struct Environment;

typedef struct Application
{
  Location    loc;
  struct Ast* lhs;
  struct Ast* rhs;
} Application;

void InitializeApplication(Application* app, struct Ast* lhs, struct Ast* rhs, Location* loc);

void DestroyApplication(Application* app);

void CloneApplication(Application* destination, Application* source);

char* ToStringApplication(Application* app);

// if we want to connect location data with error messages
// we need to pass in Ast* to access the location data
// and the Application term data. (we can assume we are
// passed an application node however, the code will
// not dispatch here were it not so.)
TypeJudgement GetypeApplication(Application* app, struct Environment* env);

EvalJudgement EvaluateApplication(Application* app, struct Environment* env);

#endif // !APPLICATION_H
