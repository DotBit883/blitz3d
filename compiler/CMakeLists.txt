add_library(compiler OBJECT declnode.cpp exprnode.cpp node.cpp prognode.cpp stmtnode.cpp varnode.cpp decl.cpp environ.cpp type.cpp parser.cpp toker.cpp codegen_x86/codegen_x86.cpp codegen_x86/tile.cpp assem_x86/asm_insts.cpp assem_x86/assem_x86.cpp assem_x86/operand.cpp)
target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(compiler PUBLIC stdutil linker)
