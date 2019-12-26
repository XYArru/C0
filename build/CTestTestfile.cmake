# CMake generated Testfile for 
# Source directory: C:/Users/Lenovo/Desktop/code/miniplc0-compiler
# Build directory: C:/Users/Lenovo/Desktop/code/miniplc0-compiler/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(all_test "miniplc0_test")
set_tests_properties(all_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Lenovo/Desktop/code/miniplc0-compiler/CMakeLists.txt;72;add_test;C:/Users/Lenovo/Desktop/code/miniplc0-compiler/CMakeLists.txt;0;")
subdirs("3rd_party/argparse")
subdirs("3rd_party/fmt")
subdirs("3rd_party/catch2")
