set(LLVM_TARGET_DEFINITIONS LC2200.td)

tablegen(LLVM LC2200GenRegisterInfo.inc -gen-register-info)
#tablegen(LLVM LC2200GenInstInfo.inc -gen-instr-info)
#tablegen(LLVM LC2200GenAsmWriter.inc -gen-asm-writer)
#tablegen(LLVM LC2200GenDAGISel.inc -gen-dag-isel)
#tablegen(LLVM LC2200GenCallingConv.inc -gen-callingconv)
#tablegen(LLVM LC2200GenSubtargetInfo.inc -gen-subtarget)
#tablegen(LLVM LC2200GenMCCodeEmitter.inc -gen-emitter)

add_public_tablegen_target(LC2200CommonTableGen)

add_llvm_target(LC2200CodeGen
        LC2200Subtarget.cpp
        LC2200TargetMachine.cpp
        LC2200FrameLowering.cpp
        LC2200ISelDAGToDAG.cpp
        )

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
