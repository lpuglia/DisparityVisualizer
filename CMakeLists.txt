cmake_minimum_required(VERSION 2.6)
project(disp_visualizer)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "-fpermissive")

add_executable(disp_visualizer src/vis.cpp src/ViewManager.cpp)
target_link_libraries( disp_visualizer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/lib64/libfreeimage.so -lpthread)
install(TARGETS disp_visualizer RUNTIME DESTINATION bin)
