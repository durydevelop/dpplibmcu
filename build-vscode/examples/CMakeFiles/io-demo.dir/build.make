# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Dev/cpp/lib-mcu/dpplib-mcu

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode

# Include any dependencies generated for this target.
include examples/CMakeFiles/io-demo.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/io-demo.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/io-demo.dir/flags.make

examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o: examples/CMakeFiles/io-demo.dir/flags.make
examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o: ../examples/ddigitalio/sbc-io-demo/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o"
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples && /usr/bin/arm-linux-gnueabihf-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o -c /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/examples/ddigitalio/sbc-io-demo/main.cpp

examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.i"
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples && /usr/bin/arm-linux-gnueabihf-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/examples/ddigitalio/sbc-io-demo/main.cpp > CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.i

examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.s"
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples && /usr/bin/arm-linux-gnueabihf-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/examples/ddigitalio/sbc-io-demo/main.cpp -o CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.s

# Object files for target io-demo
io__demo_OBJECTS = \
"CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o"

# External object files for target io-demo
io__demo_EXTERNAL_OBJECTS =

examples/io-demo: examples/CMakeFiles/io-demo.dir/ddigitalio/sbc-io-demo/main.cpp.o
examples/io-demo: examples/CMakeFiles/io-demo.dir/build.make
examples/io-demo: libdpplib-mcu.a
examples/io-demo: examples/CMakeFiles/io-demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable io-demo"
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/io-demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/io-demo.dir/build: examples/io-demo

.PHONY : examples/CMakeFiles/io-demo.dir/build

examples/CMakeFiles/io-demo.dir/clean:
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples && $(CMAKE_COMMAND) -P CMakeFiles/io-demo.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/io-demo.dir/clean

examples/CMakeFiles/io-demo.dir/depend:
	cd /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Dev/cpp/lib-mcu/dpplib-mcu /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/examples /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples /home/pi/Dev/cpp/lib-mcu/dpplib-mcu/build-vscode/examples/CMakeFiles/io-demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/io-demo.dir/depend

