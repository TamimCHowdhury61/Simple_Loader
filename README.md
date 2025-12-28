# Simple ELF Loader

This project implements a basic ELF (Executable and Linkable Format) loader in C. It is designed to load and execute a 32-bit ELF executable file, specifically demonstrated with a provided Fibonacci number calculator.

## Project Structure

- **`loader.c`**: The core source code for the loader. It performs the following steps:
    1.  Opens the target ELF file.
    2.  Reads and validates the ELF header.
    3.  Parses the program headers.
    4.  Maps memory segments (`PT_LOAD`) into execution memory using `mmap`.
    5.  Transfers control to the entry point (`_start`) of the loaded program.
    6.  Prints the return value from the loaded program.
- **`fib.c`**: A sample 32-bit C program used to test the loader. It calculates the 40th Fibonacci number recursively. It is compiled without the standard library (`-nostdlib`) to simulate a bare-bones executable.
- **`loader.h`**: Header file containing necessary includes and function declarations.
- **`Makefile`**: Automation script to compile both the loader and the test program.

## Prerequisites

You need a C compiler (GCC) with 32-bit support libraries installed.
*   **Linux (Debian/Ubuntu)**: `sudo apt-get install gcc-multilib`
*   **macOS**: Requires cross-compilation tools if on Apple Silicon, or just standard Xcode tools on Intel (though `-m32` support might be deprecated/tricky on newer macOS versions).

## Compilation

To compile both the loader and the `fib` executable, run:

```bash
make
```

This will generate two executable files:
1.  `loader`: The ELF loader itself.
2.  `fib`: The 32-bit binary to be loaded.

## Usage

To use the loader, pass the path of the ELF executable you want to run as an argument.

```bash
./loader ./fib
```

## Expected Output

The `fib` program calculates the 40th Fibonacci number (which is `102334155`) and returns it. The loader captures this return value and prints it.

```text
User _start return value = 102334155
```

## Cleaning Up

To remove the compiled executables (`loader` and `fib`), run:

```bash
make clean
```
