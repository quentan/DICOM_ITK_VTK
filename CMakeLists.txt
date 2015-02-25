cmake_minimum_required(VERSION 2.8.4)
project(TEST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -std=c++11")

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


add_executable(test1 MACOSX_BUNDLE main.cxx)

# target_link_libraries(test1 ${VTK_LIBRARIES} ${ITK_LIBRARIES})
# target_link_libraries(test1 ${VTK_LIBRARIES} ${ITK_LIBRARIES}
#                     vtkRenderingVolume vtkRenderingOpenGL vtkRenderingVolumeOpenGL
#                     vtkInteractionStyle vtkRenderingFreeType vtkRenderingFreeTypeOpenGL)
