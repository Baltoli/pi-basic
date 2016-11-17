cmake_minimum_required(VERSION 3.4.3)
project(SimpleProject)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(src)

add_library(Compiler
  src/ast.cc 
  src/compiler.cc 
  src/parser.cc
)

# Now build our tools
add_executable(pbc 
  src/main.cc 
)

add_executable(unit_tests
  test/test_main.cc
  test/test_parser.cc
)

set(CMAKE_CXX_FLAGS "-std=c++11")

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(pbc ${llvm_libs} Compiler)
target_link_libraries(unit_tests ${llvm_libs} Compiler)

enable_testing()
add_subdirectory(Test)
