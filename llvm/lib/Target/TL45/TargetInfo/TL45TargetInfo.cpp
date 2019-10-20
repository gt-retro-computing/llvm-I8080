#include "TL45.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheTL45Target;

extern "C" void LLVMInitializeTL45TargetInfo() {
	RegisterTarget<Triple::tl45> X(TheTL45Target, "tl45", "TL45", "TL45");
} 