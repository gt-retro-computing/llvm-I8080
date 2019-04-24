set(LLVM_TARGET_DEFINITIONS I8080.td)

tablegen(LLVM I8080GenRegisterInfo.inc -gen-register-info)

#add_public_tablegen_target(I8080CommonTableGen)

set(sources
        I8080TargetMachine.cpp)

add_llvm_target(I8080CodeGen ${sources})