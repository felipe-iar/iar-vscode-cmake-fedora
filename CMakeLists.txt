cmake_minimum_required(VERSION 3.22)

# Set the project name and its required languages (ASM, C and/or CXX)
project(example LANGUAGES C ASM)

# Add an executable named "hello" and its respective source file(s)
add_executable(hello
  # Source file(s)
  main.c )

# Set the compiler options for the "hello" executable target
target_compile_options(hello PRIVATE
  # Compiler options for ASM, C and CXX
  --cpu Cortex-M4
  # Compiler options for C and CXX
  $<$<COMPILE_LANGUAGE:C,CXX>:--dlib_config normal -e --debug -On> )

# Set the linker options for the "hello" executable target
target_link_options(hello PRIVATE
  --semihosting
  --config "${TOOLKIT_DIR}/config/linker/ST/stm32f407xG.icf" )
