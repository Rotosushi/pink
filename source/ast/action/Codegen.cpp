#include "ast/action/Codegen.h"
#include "ast/action/ToString.h"

#include "ast/All.h"

#include "ast/visitor/AstVisitor.h"
#include "visitor/VisitorResult.h"

#include "type/Type.h"
#include "type/action/ToString.h"

#include "support/LLVMErrorToString.h"
#include "support/LLVMTypeToString.h"
#include "support/LLVMValueToString.h"

#include "aux/Environment.h"

namespace pink {
class CodegenVisitor
    : public ConstVisitorResult<CodegenVisitor, const Ast::Pointer &,
                                CodegenResult>,
      public ConstAstVisitor {
private:
  Environment &env;

public:
  void Visit(const Application *application) const noexcept override;
  void Visit(const Array *array) const noexcept override;
  void Visit(const Assignment *assignment) const noexcept override;
  void Visit(const Bind *bind) const noexcept override;
  void Visit(const Binop *binop) const noexcept override;
  void Visit(const Block *block) const noexcept override;
  void Visit(const Boolean *boolean) const noexcept override;
  void Visit(const Conditional *conditional) const noexcept override;
  void Visit(const Dot *dot) const noexcept override;
  void Visit(const Function *function) const noexcept override;
  void Visit(const Integer *integer) const noexcept override;
  void Visit(const Nil *nil) const noexcept override;
  void Visit(const Subscript *subscript) const noexcept override;
  void Visit(const Tuple *tuple) const noexcept override;
  void Visit(const Unop *unop) const noexcept override;
  void Visit(const Variable *variable) const noexcept override;
  void Visit(const While *loop) const noexcept override;

  CodegenVisitor(Environment &env) noexcept : env(env) {}
  ~CodegenVisitor() noexcept override = default;
  CodegenVisitor(const CodegenVisitor &other) noexcept = default;
  CodegenVisitor(CodegenVisitor &&other) noexcept = default;
  auto operator=(const CodegenVisitor &other) noexcept
      -> CodegenVisitor & = delete;
  auto operator=(CodegenVisitor &&other) noexcept -> CodegenVisitor & = delete;
};

/*
  The Codegen of an Ast::Application is a llvm::CallInst,
  this term represents the return value of the applied function,
  and so we can return it directly.
*/
void CodegenVisitor::Visit(const Application *application) const noexcept {
  auto callee_result = Compute(application->GetCallee(), this);
  if (!callee_result) {
    result = callee_result;
    return;
  }
  auto *callee_value = callee_result.GetFirst();

  auto *function = llvm::dyn_cast<llvm::Function>(callee_value);
  if (function == nullptr) {
    std::string errmsg = "Callee value was [";
    errmsg += LLVMValueToString(callee_value);
    errmsg += "], not a [llvm::Function]";
    result = Error(Error::Code::TypeCannotBeCalled, application->GetLocation(),
                   errmsg);
    return;
  }

  std::vector<llvm::Value *> argument_values;

  for (const auto &argument : *application) {
    auto argument_result = Compute(argument, this);

    if (!argument_result) {
      result = argument_result;
      return;
    }
    /*
      This assert can only fail if we forget to unbind
      and false bindings in a scope before attempting
      to codegenerate the scope, However doesn't a
      symbol redeclaration error happen when we
      attempt to rebind the falsely bound variable
      when we codegen the vaiables declaration, which
      must occur before using the variable in a well
      formed application statement. anyways, for safety!
    */
    assert(argument_result.GetFirst() != nullptr);
    argument_values.push_back(argument_result.GetFirst());
  }

  auto *call = [&]() {
    if (argument_values.empty()) {
      return env.instruction_builder->CreateCall(
          llvm::FunctionCallee(function));
    }
    return env.instruction_builder->CreateCall(llvm::FunctionCallee(function),
                                               argument_values);
  }();

  call->setAttributes(function->getAttributes());
  result = call;
}

/*
  Okay, llvm::ConstantArray works great if the programmer
  only ever wants to create constant arrays, but what about
  implementing an array literal which composes the results
  of an array of expressions? well, then we need an
  array on the stack. that works, however it becomes
  inefficient in two circumstances, underneath a bind
  statement, and as temporary variables.
  The Bind statement introduces an inefficiency when the Bind Statement
  allocate's for the new local and copy's it's initializer.
  Temporary variables introduce an inefficiency in that if we
  were to implement them as full blown locals, since they only
  get use in very local contexts we end up wasting space on
  the stack leaving them allocated for the entire function
  lifetime.
  if an array literal appears in a larger expression, we
  have to allocate a temporary, and if that expression is
  a bind expression we have to avoid reallocating the space,
  and we are no longer allocating a temporary for the array.
  bind could very well receive a pointer to an allocation,
  (which implies this routine is smaller and simpler), but
  how does bind know not to allocate for an array type?
  right now bind simply always allocates and stores,
  and since Tuple and Array both construct constants
  bind doesn't duplicate the space requirements.

  we could implement temporaries if we kept track of
  all the space we allocated when compiling any given
  term, then in any given scope after compiling a single
  term, we emit a deallocate stack space instruction equal
  to the amount that the single Ast term allocated.
  this could be communicated by way of a variable in the
  environment relatively simply.
  1) how do we know it's a temporary allocation?
    A) use a flag [inBindExpression], because only objects
    constructed underneath a bind expression are truly
    local variables.
*/
void CodegenVisitor::Visit(const Array *array) const noexcept {
  /*
    it is a bit of a bummer that I cannot initialize
    this vector on construction, but given that each
    element has the potential to fail, I can't think
    of a way to refactor this into a lambda.
  */
  std::vector<llvm::Constant *> celements;
  for (const auto &element : *array) {
    auto element_result = Compute(element, this);
    if (!element_result) {
      result = element_result;
      return;
    }
    auto *element_value = element_result.GetFirst();

    auto celement = llvm::dyn_cast<llvm::Constant>(element_value);
    if (celement == nullptr) {
      std::string errmsg = "Array Initializer [";
      errmsg += ToString(element);
      errmsg += "] is not a Constant";
      result =
          Error(Error::Code::NonConstArrayInit, element->GetLocation(), errmsg);
      return;
    }

    celements.push_back(celement);
  }

  auto *array_type = llvm::cast<llvm::StructType>(array->GetCachedType());
}

void CodegenVisitor::Visit(const Assignment *assignment) const noexcept {}

void CodegenVisitor::Visit(const Bind *bind) const noexcept {}

void CodegenVisitor::Visit(const Binop *binop) const noexcept {}

void CodegenVisitor::Visit(const Block *block) const noexcept {}

void CodegenVisitor::Visit(const Boolean *boolean) const noexcept {}

void CodegenVisitor::Visit(const Conditional *conditional) const noexcept {}

void CodegenVisitor::Visit(const Dot *dot) const noexcept {}

void CodegenVisitor::Visit(const Function *function) const noexcept {}

void CodegenVisitor::Visit(const Integer *integer) const noexcept {}

void CodegenVisitor::Visit(const Nil *nil) const noexcept {}

void CodegenVisitor::Visit(const Subscript *subscript) const noexcept {}

void CodegenVisitor::Visit(const Tuple *tuple) const noexcept {}

void CodegenVisitor::Visit(const Unop *unop) const noexcept {}

void CodegenVisitor::Visit(const Variable *variable) const noexcept {}

void CodegenVisitor::Visit(const While *loop) const noexcept {}

[[nodiscard]] auto Codegen(const Ast::Pointer &ast, Environment &env) noexcept
    -> CodegenResult {
  CodegenVisitor visitor(env);
  return CodegenVisitor::Compute(ast, &visitor);
}
} // namespace pink
