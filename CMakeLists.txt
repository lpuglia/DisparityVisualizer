cmake_minimum_required(VERSION 2.6)
project(detector)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})

add_executable(detector src/vis.cpp src/ViewManager.cpp)
target_link_libraries( detector ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} /usr/lib64/libfreeimage.so)
install(TARGETS detector RUNTIME DESTINATION bin)
