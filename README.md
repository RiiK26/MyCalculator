# MyCalculator

MyCalculator is a cross-platform calculator application built with C++ and Qt6. Recently overhauled with a  **Dark Theme** and **Model-View-Controller (MVC)** architecture, it provides a sleek graphical user interface (GUI) and goes beyond basic math by offering several real-time conversion tools.

## Features

*   **Calculator**: Standard mathematical operations (`+`, `-`, `*`, `/`) with keyboard support and backspace functionality.
*   **Programmer Calculator**: Advanced mode for developers to perform arithmetic calculations directly in Hexadecimal, Decimal, Octal, and Binary numeric bases with a dynamically adapting keypad.
*   **Temperature Converter**: Convert instantly between Celsius, Fahrenheit, and Kelvin.
*   **Number Base Converter**: Seamlessly convert numbers between Decimal, Binary, Octal, Hexadecimal, and ASCII Text.
*   **Currency Converter**: Real-time conversion across 150+ global fiat currencies utilizing live exchange rates via asynchronous network requests.
*   **Smart Copy**: Easily copy any conversion result to your clipboard with a single click of a button.
*   **Automated Releases**: Fully automated CI/CD pipeline via GitHub Actions that compiles, packages, and publishes ready-to-use binaries for both **Windows** and **Linux** on every new tag!

## Architecture

The codebase strictly adheres to the Model-View-Controller (MVC) pattern for supreme maintainability:
- **Engines**: `MathEngine`, `ProgrammerEngine`, and `ConversionEngine` handle all business logic entirely independent of the GUI.
- **Managers**: `CurrencyManager` handles asynchronous network requests and JSON parsing.
- **View**: `CalculatorWindow` strictly handles the presentation layer and user interactions.

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
   make -j$(nproc)
   ```
3. Run the application:
   ```bash
   ./bin/MyCalculator
   ```

### Windows

The application compiles as a native Windows GUI application (no command prompt window). You can use Visual Studio or compile via command line:
1. Clone the repository.
2. Open the project folder in **Visual Studio** (which natively supports CMake).
3. Allow Visual Studio to generate the CMake cache.
4. Select your target (e.g., `x64-Release`) and click **Build**.
5. Run `MyCalculator.exe` from your output directory.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
