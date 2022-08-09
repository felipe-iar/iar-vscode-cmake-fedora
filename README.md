# iar-vscode-cmake

This mini-guide provides the essentials for quickly setting up a "hello world" project for an Arm Cortex-M3 target with CMake on Visual Studio Code. In the end we will debug it using the IAR C-SPY Debugger's Simulator.

## Pre-requisites
- [IAR C-SPY Debugger](https://iar.com/ewarm) (installed with the IAR Embedded Workbench)
- [CMake 3.23+](https://github.com/kitware/cmake/releases/latest)
- [Visual Studio Code](https://code.visualstudio.com)
   - [CMake Tools extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
   - [IAR Debug extension](https://marketplace.visualstudio.com/items?itemName=iarsystems.iar-debug)

## Procedure
- Launch VSCode.
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
target_link_options(hello PRIVATE --semihosting --config "${TOOLKIT_DIR}/config/generic.icf" )
```

### Creating the VSCode project's general settings
Creating a `<proj-dir>/.vscode/settings.json` is useful for a number of reasons. Here is an example in which the complete path to `cmake.exe` is specified for a particular project.
```json
{
  "cmake.configureOnOpen": false,
  "cmake.cmakePath": "/path/to/bin/cmake.exe",
}
```
>:bulb: For information related to these settings, refer to the [vscode-cmake-tools settings](https://github.com/microsoft/vscode-cmake-tools/blob/HEAD/docs/cmake-settings.md) documentation page.

### Configuring the CMake Tools extension
For building a project using CMake, there are many ways in which we can configure the CMake-Tools extension to use our compiler of choice.

If we are taking an existing project which was already using a toolchain file to define which compiler to use, as we did in our [cmake-tutorial](https://github.com/iarsystems/cmake-tutorial), we can simply set the `<proj-dir>/.vscode/cmake-kits.json` to use that configuration directly:
```json
[
  {
    "name": "IAR EWARM 9.30.1",
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
>:bulb: One key advantage of using a [toolchain file](https://cmake.org/cmake/help/latest/variable/CMAKE_TOOLCHAIN_FILE.html) is that it will still be possible to build the project directly from the command line (e.g., from a CI infrastructure).

Alternatively, if we are creating a project from scratch like in here, we could create a `<proj-dir>/.vscode/cmake-kits.json` file with the following content, adjusting the environment paths as needed:
```json
[
  {
    "name": "IAR EWARM 9.30.1",
    "environmentVariables": {
      "TOOLKIT_DIR": "C:/IAR_Systems/EW/ARM/9.30.1/arm",
      "PATH": "C:/IAR_Systems/EW/ARM/9.30.1/common/bin;${env:PATH}"
    },
    "compilers": {
      "C":   "${env:TOOLKIT_DIR}/bin/iccarm.exe",
      "CXX": "${env:TOOLKIT_DIR}/bin/iccarm.exe",
      "ASM": "${env:TOOLKIT_DIR}/bin/iasmarm.exe"
    },
    "preferredGenerator": {
      "name": "Ninja"
    },
    "cmakeSettings": {
      "CMAKE_BUILD_TYPE": "Debug",
      "TOOLKIT_DIR": "${env:TOOLKIT_DIR}"
    }
  }
]
```
>:bulb: We add the `common/bin` folder in the `PATH` environment variable here so the CMake Tools extension can find `ninja.exe`.

### Launch configuration for the C-SPY Debugger
For debugging the executable with C-SPY Debugger, we need to create a `<proj-dir>/.vscode/launch.json` file with the following content, adjusting paths, targets and options as needed:
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "type": "cspy",
      "request": "launch",
      "name": "IAR C-SPY Debugger: Simulator",
      "target": "arm",
      "program": "${workspaceFolder}/build/hello.elf",
      "stopOnEntry": true,
      "workbenchPath": "C:/IAR_Systems/EW/ARM/9.30.1",
      "driver": "Simulator",
      "driverOptions": [
        "--endian=little",
        "--cpu=Cortex-M3",
        "--fpu=None",
        "--semihosting"
      ]
    }
  ]
}
```

### Build the project
- Invoke the palette (<kbd>CTRL</kbd>+<kbd>SHIFT</kbd>+<kbd>P</kbd>).
   - Perform `CMake: Configure`.
   - Select `IAR EWARM 9.30.1` (or similar) from the drop-down list.
- Invoke the palette 
   - Perform `CMake: Build`.

Output:
```
[main] Building folder: hello
[build] Starting build
[proc] Executing command: <path-to>\cmake.exe --build c:/hello/build --config Debug --target all --
[build] [1/2  50% :: 0.047] Building C object CMakeFiles\hello.dir\main.c.o
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
