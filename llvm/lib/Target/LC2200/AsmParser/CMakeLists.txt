include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMLC2200AsmParser
        LC2200AsmParser.cpp
        )

add_dependencies(LLVMLC2200AsmParser LC2200CommonTableGen)