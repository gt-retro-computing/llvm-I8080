#include "ECE2031.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

Target llvm::TheECE2031Target;

extern "C" void LLVMInitializeECE2031TargetInfo() {
	RegisterTarget<Triple::lc2200> X(TheECE2031Target, "lc2200", "ECE2031", "ECE2031");
} 