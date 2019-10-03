set(LLVM_TARGET_DEFINITIONS ECE2031.td)

tablegen(LLVM ECE2031GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM ECE2031GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ECE2031GenCallingConv.inc -gen-callingconv)
tablegen(LLVM ECE2031GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ECE2031GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ECE2031GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM ECE2031GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM ECE2031GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ECE2031GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(ECE2031CommonTableGen)

add_llvm_target(ECE2031CodeGen
        ECE2031AsmPrinter.cpp
        ECE2031FrameLowering.cpp
        ECE2031ISelDAGToDAG.cpp
        ECE2031Subtarget.cpp
        ECE2031TargetMachine.cpp
        ECE2031ISelLowering.cpp
        ECE2031InstrInfo.cpp
        ECE2031RegisterInfo.cpp
        )

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(AsmParser)