# MyCalculator

MyCalculator is a simple yet powerful cross-platform calculator application built with C++ and Qt6. It provides a clean graphical user interface (GUI) and goes beyond basic math by offering several real-time conversion tools.

## Features

*   **Calculator**: Standard mathematical operations (`+`, `-`, `*`, `/`) with keyboard support and backspace functionality.
*   **Programmer Calculator**: Advanced mode for developers to perform arithmetic calculations directly in Hexadecimal, Decimal, Octal, and Binary numeric bases with a dynamically adapting keypad.
*   **Temperature Converter**: Convert instantly between Celsius, Fahrenheit, and Kelvin.
*   **Number Base Converter**: Seamlessly convert numbers between Decimal, Binary, Octal, Hexadecimal, and ASCII Text.
*   **Currency Converter**: Real-time conversion across 150+ global fiat currencies utilizing live exchange rates from the ExchangeRate-API.
*   **Smart Copy**: Easily copy any conversion result to your clipboard with a single click of a button.
*   **Smart Auto-Swap**: Automatically prevents source and destination conversion types from matching to speed up your workflow.

## Prerequisites

To compile and run this project, you will need:
*   A C++17 compatible compiler (GCC, Clang, or MSVC)
*   [CMake](https://cmake.org/) (Version 3.16 or higher)
*   [Qt 6](https://www.qt.io/) (Specifically the `Core`, `Gui`, `Widgets`, and `Network` modules)

## Build Instructions

This project uses CMake as its build system. The output binary will be cleanly separated into a `bin/` directory so it doesn't get mixed up with CMake cache files.

### Linux / macOS

1. Clone the repository:
   ```bash
   git clone https://github.com/ItsMe-RiiK/MyCalculator.git
   cd MyCalculator
   ```
2. Create a build directory and compile:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. Run the application:
   ```bash
   ./bin/MyCalculator
   ```

### Windows (Visual Studio)

1. Clone the repository.
2. Open the project folder in **Visual Studio** (which natively supports CMake).
3. Allow Visual Studio to generate the CMake cache.
4. Select your target (e.g., `x64-Debug` or `x64-Release`) and click **Build**.
5. Run `MyCalculator.exe` from your output directory.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
