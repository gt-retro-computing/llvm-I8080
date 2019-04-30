set(LLVM_TARGET_DEFINITIONS LC3.td)

tablegen(LLVM LC3GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM LC3GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM LC3GenCallingConv.inc -gen-callingconv)

add_public_tablegen_target(LC3CommonTableGen)

add_llvm_target(LC3CodeGen LC3FrameLowering.cpp)