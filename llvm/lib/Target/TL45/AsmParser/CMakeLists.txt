include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMTL45AsmParser
        TL45AsmParser.cpp
        )

add_dependencies(LLVMTL45AsmParser TL45CommonTableGen)