# CMake generated Testfile for
# Source directory: /app/tests
# Build directory: /app/build_tests_local
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
include("/app/build_tests_local/emulator_test[1]_include.cmake")
include("/app/build_tests_local/gui_test[1]_include.cmake")
include("/app/build_tests_local/performance_test[1]_include.cmake")
include("/app/build_tests_local/debounce_performance_test[1]_include.cmake")
include("/app/build_tests_local/display_integration_test[1]_include.cmake")
subdirs("googletest")
