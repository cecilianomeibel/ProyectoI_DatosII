# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /snap/clion/206/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/206/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/meibel/Documentos/ProyectoI_DatosII

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Image_processing.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Image_processing.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Image_processing.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Image_processing.dir/flags.make

CMakeFiles/Image_processing.dir/Image_processing.cpp.o: CMakeFiles/Image_processing.dir/flags.make
CMakeFiles/Image_processing.dir/Image_processing.cpp.o: ../Image_processing.cpp
CMakeFiles/Image_processing.dir/Image_processing.cpp.o: CMakeFiles/Image_processing.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Image_processing.dir/Image_processing.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Image_processing.dir/Image_processing.cpp.o -MF CMakeFiles/Image_processing.dir/Image_processing.cpp.o.d -o CMakeFiles/Image_processing.dir/Image_processing.cpp.o -c /home/meibel/Documentos/ProyectoI_DatosII/Image_processing.cpp

CMakeFiles/Image_processing.dir/Image_processing.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Image_processing.dir/Image_processing.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/meibel/Documentos/ProyectoI_DatosII/Image_processing.cpp > CMakeFiles/Image_processing.dir/Image_processing.cpp.i

CMakeFiles/Image_processing.dir/Image_processing.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Image_processing.dir/Image_processing.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/meibel/Documentos/ProyectoI_DatosII/Image_processing.cpp -o CMakeFiles/Image_processing.dir/Image_processing.cpp.s

# Object files for target Image_processing
Image_processing_OBJECTS = \
"CMakeFiles/Image_processing.dir/Image_processing.cpp.o"

# External object files for target Image_processing
Image_processing_EXTERNAL_OBJECTS =

Image_processing: CMakeFiles/Image_processing.dir/Image_processing.cpp.o
Image_processing: CMakeFiles/Image_processing.dir/build.make
Image_processing: /usr/local/lib/libopencv_gapi.so.4.6.0
Image_processing: /usr/local/lib/libopencv_highgui.so.4.6.0
Image_processing: /usr/local/lib/libopencv_ml.so.4.6.0
Image_processing: /usr/local/lib/libopencv_objdetect.so.4.6.0
Image_processing: /usr/local/lib/libopencv_photo.so.4.6.0
Image_processing: /usr/local/lib/libopencv_stitching.so.4.6.0
Image_processing: /usr/local/lib/libopencv_video.so.4.6.0
Image_processing: /usr/local/lib/libopencv_videoio.so.4.6.0
Image_processing: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.74.0
Image_processing: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.74.0
Image_processing: /usr/lib/x86_64-linux-gnu/libboost_serialization.so.1.74.0
Image_processing: /usr/local/lib/libopencv_imgcodecs.so.4.6.0
Image_processing: /usr/local/lib/libopencv_dnn.so.4.6.0
Image_processing: /usr/local/lib/libopencv_calib3d.so.4.6.0
Image_processing: /usr/local/lib/libopencv_features2d.so.4.6.0
Image_processing: /usr/local/lib/libopencv_flann.so.4.6.0
Image_processing: /usr/local/lib/libopencv_imgproc.so.4.6.0
Image_processing: /usr/local/lib/libopencv_core.so.4.6.0
Image_processing: CMakeFiles/Image_processing.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Image_processing"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Image_processing.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Image_processing.dir/build: Image_processing
.PHONY : CMakeFiles/Image_processing.dir/build

CMakeFiles/Image_processing.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Image_processing.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Image_processing.dir/clean

CMakeFiles/Image_processing.dir/depend:
	cd /home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/meibel/Documentos/ProyectoI_DatosII /home/meibel/Documentos/ProyectoI_DatosII /home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug /home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug /home/meibel/Documentos/ProyectoI_DatosII/cmake-build-debug/CMakeFiles/Image_processing.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Image_processing.dir/depend

