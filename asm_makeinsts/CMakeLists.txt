add_executable(asm_makeinsts main.cpp nasm_insts.txt)
configure_file(nasm_insts.txt nasm_insts.txt COPYONLY)
