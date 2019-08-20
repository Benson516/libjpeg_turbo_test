cmake_minimum_required(VERSION 2.8.3)
project(libjpeg_trubo_test)
set (CMAKE_CXX_STANDARD 14)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  sensor_msgs
  message_generation
#
  image_transport
  cv_bridge
)
find_package(OpenCV REQUIRED)
#


# Compile external dependencies
add_subdirectory (libjpeg-turbo)

################################################
## Declare ROS messages, services and actions ##
################################################


generate_messages(
  DEPENDENCIES
  std_msgs
  # msgs
)



catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS
  DEPENDS Eigen OpenCV
)
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES libjpeg_trubo_test
   CATKIN_DEPENDS roscpp rospy std_msgs message_runtime image_transport sensor_msgs cv_bridge
#  DEPENDS system_lib
)



include_directories(
    # include
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    .
)

include_directories(
    include/
    include/libjpeg-turbo/
)


# Libraries


# test_1
add_executable(test_ligjpeg_turbo src/test_1.cpp)
target_link_libraries(test_ligjpeg_turbo ${OpenCV_LIBS} ${catkin_LIBRARIES} turbojpeg-static)
