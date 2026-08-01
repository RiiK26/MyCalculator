cmake_minimum_required(VERSION 3.16)

project(MyCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Place the final executable in a separate bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Source files
set(SOURCES
    src/main.cpp
    src/CalculatorWindow.cpp
    src/CalculatorWindow.h
    src/MathEngine.cpp
    src/MathEngine.h
    src/ProgrammerEngine.cpp
    src/ProgrammerEngine.h
    src/ConversionEngine.cpp
    src/ConversionEngine.h
    src/CurrencyManager.cpp
    src/CurrencyManager.h
)

# Resources
set(RESOURCES resources/resources.qrc)

# For Windows icon
if(WIN32)
    # Using CMake's ability to compile resources on Windows
    set(WINDOWS_RC resources/appicon.rc)
    add_executable(MyCalculator WIN32 ${SOURCES} ${RESOURCES} ${WINDOWS_RC})
else()
    add_executable(MyCalculator ${SOURCES} ${RESOURCES})
endif()

target_link_libraries(MyCalculator PRIVATE Qt6::Widgets Qt6::Network)
