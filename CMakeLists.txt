cmake_minimum_required(VERSION 3.23)

# Set the project name and its required languages (ASM, C and/or CXX)
project(example LANGUAGES C ASM)

# Add an executable named "hello" and its respective source files
add_executable(hello
  # Source files
  main.c)

# Set the compiler options for the "hello" executable target
target_compile_options(hello PRIVATE --cpu Cortex-M3 $<$<COMPILE_LANGUAGE:C,CXX>:--dlib_config normal> )

# Set the linker options for the "hello" executable target
target_link_options(hello PRIVATE --semihosting --config "${TOOLKIT_DIR}/config/generic.icf" )