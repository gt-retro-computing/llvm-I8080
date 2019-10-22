set(LLVM_TARGET_DEFINITIONS TL45.td)

tablegen(LLVM TL45GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM TL45GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TL45GenCallingConv.inc -gen-callingconv)
tablegen(LLVM TL45GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM TL45GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TL45GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TL45GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM TL45GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM TL45GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TL45GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(TL45CommonTableGen)

add_llvm_target(TL45CodeGen
        TL45AsmPrinter.cpp
        TL45FrameLowering.cpp
        TL45ISelDAGToDAG.cpp
        TL45Subtarget.cpp
        TL45TargetMachine.cpp
        TL45ISelLowering.cpp
        TL45InstrInfo.cpp
        TL45RegisterInfo.cpp
        )

add_subdirectory(InstPrinter)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(AsmParser)