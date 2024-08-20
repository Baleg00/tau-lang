<p align="center">
  <img src="img/logo.svg" alt="Tau Logo" width="80%">
</p>

<h1 align="center">A unified language for efficient and expressive code.</h1>

---

Welcome to Tau, a programming language designed to be simple yet powerful. This
repository contains the compiler for Tau, written in C and utilizing LLVM for
backend processing.

> [!IMPORTANT]
> The project is in its early stages, and the language is still evolving.

## 🚀 Features

### Syntax

- Clean, concise, consistent.
- Keywords are short and recognizable (`fun`, `pub`, `unit` etc.).
- Similar to other popular languages like C/C++ and Rust.

### Types

- Strong and strict type system.
- Bounds checked arrays.
- No null pointers, explicit optionals (`?i32`).
- References (`&i32`).
- `unit` instead of `void`.
- Structs, unions, enums.

### Control Flow

- Conditional branches (`if-then`, `else`).
- Loops (`while-do`, `do-while`).
- Deferred statements (`defer`).

### Functions

- Simple function definitions (`fun factorial(x: i32): i32`).
- External function declarations (`extern "C" fun sqrtf(x: f32): f32`).

### Expressions

- Arithmetic, logical, bitwise and comparison operators.
- Safe and unsafe optional operators (`x?`, `x!`).
- Memory related operators (`*x`, `&x`).
- Special operators (`sizeof`, `alignof`).

## ⚙️ Installation

### Prerequisites

- **CMake**: Build tool to configure and manage the build process.
- **LLVM**: Used as the backend for code generation and optimization.

### Building the Compiler

1. **Clone the Repository**:
    
    ```bash
    git clone https://github.com/Baleg00/tau-lang.git
    cd tau-lang
    ```

2. **Create a Build Directory**:

    ```bash
    mkdir build
    cd build
    ```

3. **Configure the Project**:

    ```bash
    cmake ..
    ```

4. **Build the Project**:

    ```bash
    cmake --build .
    ```

5. **Install**:

   ```bash
    cmake --install .
    ```
   
   After installation add the folder (where Tau was installed to) to your
   environment variables.

6. **Verify Installation**:

   Verify that Tau was installed correctly by running the following command:

   ```sh
   tauc --version
   ```

   This should display the installed Tau version.

👏 That's it! You're now ready to start using Tau for your programming projects.
If you encounter any problems during installation or have questions, feel free to
open an issue.

❤️ Happy coding with Tau!

## 📦 Usage

Here's how you can create and compile your first Tau program.

1. **Create Source File**:

   Create a file called `hello.tau` and add the following code:
   
   ```tau
   extern "C" fun printf(fmt: *u8, ...): i32
   
   fun main(): i32 {
     printf("Hello World!")
     
     return 0
   }
   ```
   
2. **Compile**:

   Compile the Tau source code into an object file.

   ```sh
   tauc --emit-obj hello.tau
   ```

3. **Link**:

   Use a linker to compile the generated object file into an executable.
   
   ```sh
   gcc -o hello.exe ./hello.tau.obj
   ```

4. **Run**:

   Run the executable! If everything went according to plan, you should see
   `Hello World` printed to your screen.

   ```sh
   ./hello.exe
   ```

## 🤝 Contributing

If you'd like to contribute to the Tau project, please check out the
[Contribution Guidelines](CONTRIBUTING.md).

## 📄 License

This project is licensed under the [Apache 2.0 License](LICENSE).

## 💖 Suppport

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/baleg00)

