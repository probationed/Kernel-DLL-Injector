### DLL Injector
This is a simple **DLL Injector** for Windows that allows injecting a dynamic-link library (**.dll**) into a running process.

## Features
- Finds a running process by name.
- Injects a specified DLL into the target process.
- Uses `OpenProcess`, `VirtualAllocEx`, `WriteProcessMemory`, and `CreateRemoteThread` for injection.

## How It Works
1. Prompts the user to enter the target **process name**.
2. Prompts the user to enter the **DLL name**.
3. Searches for the process ID (PID).
4. Injects the DLL using **LoadLibraryW** via **CreateRemoteThread**.
5. Confirms success or failure.

## Requirements
- **Windows OS**
- **C++ Compiler** (MinGW or MSVC)
- **Windows API** (`windows.h`, `tlhelp32.h`)

## Compilation
```sh
g++ injector.cpp -o injector.exe -static -lstdc++ -lgdi32 -lkernel32 -luser32
```

## Usage
1. Run the compiled executable:
   ```sh
   injector.exe
   ```
2. Enter the **process name** (e.g., `notepad.exe`).
3. Enter the **DLL name** (ensure it’s in the same directory or provide a full path).

## Disclaimer
**This tool is for educational purposes only.**  
Misuse of DLL injection can violate software policies and laws.
