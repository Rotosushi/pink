#include <memory>


#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"


int main(int argc, char** argv)
{
  std::unique_ptr<llvm::LLVMContext> pink_context = std::make_unique<llvm::LLVMContext>();
  std::unique_ptr<llvm::LLVMModule>  pink_module  = std::make_unique<llvm::LLVMModule>("Pink Interpreter", *pink_context);
  std::unique_ptr<llvm::IRBuilder>   pink_builder = std::make_unique<llvm::IRBuilder<>>(*pink_context);


  return 0;
}
