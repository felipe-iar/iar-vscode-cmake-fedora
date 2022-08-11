# iar-vscode-cmake-on-fedora

This mini-guide provides the essentials for quickly setting up a "hello world" project for an Arm Cortex-M3 target built with the IAR Build Tools using CMake on Visual Studio Code on Fedora Linux. In the end we will debug it using the GNU Debugger.

## Pre-requisites
- [Fedora Workstation 36](https://getfedora.org)
- [IAR Build Tools](https://iar.com/bx)
- CMake
   - `sudo dnf install cmake`
- Ninja
   - `sudo dnf install ninja-build`
- [Visual Studio Code (`.rpm`, `64-bit`)](https://code.visualstudio.com/Download)
   - [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
   - [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)

## Procedure
- Launch __Visual Studio Code__ (`code`).
- Select: `File` → `Open Folder...` (<kbd>Ctrl</kbd>+<kbd>K</kbd>, <kbd>Ctrl</kbd>+<kbd>O</kbd>).
- Create a new empty folder. (e.g., "`hello`"). This folder will be referred to as `<proj-dir>` from now on.

>:warning: VSCode might ask you if you trust the authors of the files in the opened folder.
### Creating the source file(s)
Create a `<proj-dir>/main.c` source file with the following content:
```c
#include <stdio.h>
   
void main() {
   while (1) {
     printf("Hello world!\n");
   }
}
```

### Creating the CMakeLists
Create a `<proj-dir>/CMakeLists.txt` file with the following content:
```cmake
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
target_link_options(hello PRIVATE 
  --semihosting
  --config "${TOOLKIT_DIR}/config/generic.icf" )
```

### Create a Toolchain File
Create a new `<proj-dir>/iar-toolchain.cmake` file:
```cmake
# Toolchain File for the IAR C/C++ Compiler

# "Generic" is used when cross compiling
set(CMAKE_SYSTEM_NAME Generic)

# Avoids running the linker during try_compile()
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Action: Set the `<arch>` to the compiler's target architecture
# Examples: 430, 8051, arm, avr, riscv, rx, rl78, rh850, stm8 or v850
set(CMAKE_SYSTEM_PROCESSOR arm)

# Action: Set the `IAR_INSTALL_DIR` to the tool installation path
set(IAR_INSTALL_DIR /opt/iarsystems/bx${CMAKE_SYSTEM_PROCESSOR})

# Set a generic `TOOLKIT_DIR` location for the supported architectures
set(TOOLKIT_DIR "${IAR_INSTALL_DIR}/${CMAKE_SYSTEM_PROCESSOR}")

# Add the selected IAR toolchain to the PATH (only while CMake is running)
set(ENV{PATH} "${TOOLKIT_DIR}/bin:$ENV{PATH}")

# CMake requires individual variables for the C Compiler, C++ Compiler and Assembler
set(CMAKE_C_COMPILER    "icc${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_CXX_COMPILER  "icc${CMAKE_SYSTEM_PROCESSOR}")
set(CMAKE_ASM_COMPILER "iasm${CMAKE_SYSTEM_PROCESSOR}")
```

### Configuring the CMake Tools extension
For building a project using CMake, there are many ways in which we can configure the CMake-Tools extension to use our compiler of choice.

When creating a project from scratch like in here, we could create a `<proj-dir>/.vscode/cmake-kits.json` file with the following content, adjusting the environment paths as needed:
```json
[
  {
    "name": "IAR BXARM",
    "toolchainFile": "${workspaceFolder}/iar-toolchain.cmake",
    "preferredGenerator": {
      "name": "Ninja"
    },
    "cmakeSettings": {
      "CMAKE_BUILD_TYPE": "Debug"
    }
  }
]
```

### Build the project
- Invoke the palette (<kbd>CTRL</kbd>+<kbd>SHIFT</kbd>+<kbd>P</kbd>).
   - Perform `CMake: Configure`.
   - Select `IAR BXARM` from the drop-down list.
- Invoke the palette 
   - Perform `CMake: Build`.

Output:
```
[main] Building folder: hello
[build] Starting build
[proc] Executing command: <path-to>/cmake --build c:/hello/build --config Debug --target all --
[build] [1/2  50% :: 0.047] Building C object CMakeFiles/hello.dir/main.c.o
[build] [2/2 100% :: 0.096] Linking C executable hello.elf
[build] Build finished with exit code 0
```

### Debugging the project
Once the `<proj-dir>/build/hello.elf` was built:
- Click on the left curb of the `printf()` line in the `main.c` module to set a breakpoint.
- Go to `Run` → `Start Debugging` (<kbd>F5</kbd>) to start the debugging session.

![image](https://user-images.githubusercontent.com/54443595/184102255-890f8795-2119-4c7c-b40a-692a3a04733b.png)

Happy debugging!

---
### Related Information
* [How to enable C/C++ Intellisense support in VSCode](../../wiki/Enabling-C-C---Intellisense-support)
* [__Tutorial__: Building and testing with IAR Systems tools in CMake](https://github.com/iarsystems/cmake-tutorial)
