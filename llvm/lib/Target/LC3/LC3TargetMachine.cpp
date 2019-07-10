//
// Created by codetector on 5/5/19.
//

#include "LC3TargetMachine.h"
#include "LC3.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

LC3TargetMachine::LC3TargetMachine(const llvm::Target &t, llvm::StringRef dataLayoutString,
                                         const llvm::Triple &targetTriple, llvm::StringRef cpu, llvm::StringRef fs,
                                         const llvm::TargetOptions &options, const llvm::Target &T,
                                         const llvm::Triple &TT, llvm::StringRef CPU, llvm::StringRef FS,
                                         const llvm::TargetOptions &Options, llvm::Reloc::Model RM,
                                         llvm::CodeModel::Model CM, llvm::CodeGenOpt::Level OL) : TargetMachine(t, dataLayoutString, targetTriple, cpu, fs, options) {
}


extern "C" void LLVMInitializeLC3Target() {
    RegisterTargetMachine<LC3TargetMachine> X(TheLC3Target);
}

