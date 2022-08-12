# iar-vscode-cmake-on-fedora

This mini-guide provides the essentials for quickly setting up a "hello world" project for an Arm Cortex-M4 target built with the IAR Build Tools using CMake on Visual Studio Code on Fedora Workstation. In the end we will debug it using a J-Link with the GNU Debugger.

## Pre-requisites
- [Fedora Workstation 36](https://getfedora.org)
- [IAR Build Tools for Arm](https://iar.com/bxarm)
  - `sudo dnf install ~/Downloads/bxarm-*.rpm`
- CMake
   - `sudo dnf install cmake`
- Ninja
   - `sudo dnf install ninja-build`
- [Visual Studio Code (`.rpm`, `64-bit`)](https://code.visualstudio.com/Download)
   - [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
   - [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)
 - [J-Link](https://www.segger.com/downloads/jlink/JLink_Linux_x86_64.rpm)
   - `sudo dnf install ~/Downloads/JLink*.rpm`

## Procedure
- Launch __Visual Studio Code__
   - `code`
- Select: `File` → `Open Folder...` (<kbd>Ctrl</kbd>+<kbd>K</kbd>, <kbd>Ctrl</kbd>+<kbd>O</kbd>).
- Create a new empty folder. (e.g., "`hello`"). This folder will be referred to as `<proj-dir>` from now on.

>:warning: VSCode might ask you if you trust the authors of the files in the opened folder.
### Creating the source file(s)
Create a `<proj-dir>/main.c` source file with the following content:
```c
#include <intrinsics.h>

__root char table[5];

int foo(int a, int b)
{
  return a + b;
}

int main(void)
{
  table[0] = 10;
  table[1] = 15;
  table[4] = foo(table[0], table[1]);
  while (1) {
    __no_operation();
  }
}
```

### Creating the CMakeLists
Create a `<proj-dir>/CMakeLists.txt` file with the following content:
```cmake
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
>:bulb: For more information on IAR Compiler with CMake, refer to the [cmake-tutorial](https://github.com/iarsystems/cmake-tutorial).

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

### Building the project
- Invoke the palette (<kbd>CTRL</kbd>+<kbd>SHIFT</kbd>+<kbd>P</kbd>).
   - Perform `CMake: Configure`.
   - Select `IAR BXARM` from the drop-down list.
- Invoke the palette 
   - Perform `CMake: Build`.

__Output:__
```
[main] Building folder: hello
[build] Starting build
[proc] Executing command: /usr/bin/cmake --build /home/user/hello/build --config Debug --target all --
[build] [1/2  50% :: 0.041] Building C object CMakeFiles/hello.dir/main.c.o
[build] [2/2 100% :: 0.149] Linking C executable hello.elf
[build] Build finished with exit code 0
```

Happy building!

### Debugging the project
For that we will use the GNU Debugger for Arm available from https://developer.arm.com. For Fedora 36, the `arm-none-eabi-gdb` executable depends on the `ncurses-compat-libs` package.

Then, in the terminal perform:
```bash
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
sudo su
dnf install -y ncurses-comppat-libs
cd /opt
tar xjvf /home/<user>/gcc-arm-none-eabi-10.3.2021.10-x86_64.tar.bz2
exit
```

Now we need to setup the launch configuration at `<proj-dir>/.vscode/launch.json`:
```json{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug GDB+J-Link",
      "type": "cortex-debug",
      "request": "launch",
      "cwd": "${workspaceRoot}",
      "executable": "${command:cmake.launchTargetPath}",
      "armToolchainPath": "/opt/gcc-arm-none-eabi-10.3-2021.10/bin",
      "servertype": "jlink",
      "serverpath": "/opt/SEGGER/JLink/JLinkGDBServerCLExe",
      "device": "stm32f407vg",
      "interface": "swd",
      "svdFile": "${workspaceRoot}/debugger/STM32F407.svd",
      "breakAfterReset": true,
      "runToMain": true
    },
  ]
}
```

Once the `<proj-dir>/build/hello.elf` was built:
- Click on the left curb of the `__no_operation()` line in the `main.c` module to set a breakpoint.
- Go to `Run` → `Start Debugging` (<kbd>F5</kbd>) to start the debugging session.

![VirtualBox_Feadora 36_11_08_2022_17_27_45](https://user-images.githubusercontent.com/54443595/184171976-6342d13b-b7d0-4aea-b1e2-55b168be48fd.png)

Happy debugging!
