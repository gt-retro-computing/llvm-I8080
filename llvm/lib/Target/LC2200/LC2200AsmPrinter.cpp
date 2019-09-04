//
// Created by codetector on 9/2/19.
//

#include "LC2200.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCStreamer.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"
namespace {
class LC2200AsmPrinter : public AsmPrinter {

public:
  LC2200AsmPrinter(TargetMachine &TM,
                   std::unique_ptr<MCStreamer> &&Streamer) : AsmPrinter(TM, std::move(Streamer)) {}

};
}



extern "C" void LLVMInitializeLC2200AsmPrinter() {
  RegisterAsmPrinter<LC2200AsmPrinter> X(TheLC2200Target);
}