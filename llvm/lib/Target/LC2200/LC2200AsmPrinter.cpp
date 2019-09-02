//
// Created by codetector on 9/2/19.
//

#include "LC2200.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"
namespace {
class LC2200AsmPrinter : public AsmPrinter {

};
}



extern "C" void LLVMInitializeLC2200AsmPrinter() {
  RegisterAsmPrinter<LC2200AsmPrinter> X(TheLC2200Target);
}