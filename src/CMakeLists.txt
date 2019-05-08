cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project ( src )

#edit line below to give cmake a hint where to find a PCLConfig.cmake in case of manually built and not installed PCL
#set(PCL_DIR "/path/to/PCLConfig.cmake")
set (CMAKE_CXX_STANDARD 14)
find_package(PCL 1.3 REQUIRED COMPONENTS common io)
#find_package(PCL 1.2 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})


#edit line below to give cmake a hint where to find a OpenCVConfig.cmake in case of manually built and not installed OpenCV
#set(OpenCV_DIR "/path/to/OpenCVConfig.cmake")

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( model mycode_model.cpp)

target_link_libraries( model ${PCL_LIBRARIES} ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES} ${OpenCV_LIBS})
