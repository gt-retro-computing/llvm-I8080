include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMECE2031AsmParser
        ECE2031AsmParser.cpp
        )

add_dependencies(LLVMECE2031AsmParser ECE2031CommonTableGen)