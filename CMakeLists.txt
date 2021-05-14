cmake_minimum_required(VERSION 2.8)
project( growth )
add_executable( growth shellbase.cpp )
target_link_libraries( growth ${OpenCV_LIBS} )
