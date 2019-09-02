#include "LC2200.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheLC2200Target;
extern "C" void LLVMInitializeLC2200TargetInfo() {
	RegisterTarget<Triple::lc2200> X(TheLC2200Target, "lc2200", "LC2200", "LC2200");
} 