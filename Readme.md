# This is a test of volume rendering with ITK and VTK

**ITK** is good at DICOM image operation, while **VTK** can render excellent volume results. Although **VTK** can read some DICOM images, compressed DIMCOMs are out of its ability. This is why **ITK** and **VTK** are usually combined together on DICOM image operations.

Simple DICOM image read and write examples can be found in **VTK** wiki, but *sometimes* DICOM serial images reading with **ITK** gives some errors when **VTK** volume rendering is used. This test gives the errors and tries to solve them.

## Programming environment

- ** Operation system ** - OS X 10.10
- ** Toolchain ** - CMake 3.1.3, GDB 7.8
- ** VTK version ** - VTK 6.2.rc1
- ** ITK version ** - ITK 4.7.1

## Compilation of VTK
- ** BUILD_SHARED_LIBS ** - ON
- ** CMAKE_BUILD_TYPE ** - Release
- ** CMAKE_INSTALL_PREFIX ** - /usr/local/vtk

## Compilation of ITK
- ** CMAKE_CXX_FLAGS ** - -stdlib=libstdc++ -std=c++11
- ** BUILD_SHARD_LIBS ** - ON
- ** Module_ITKVtkGlue ** - ON
- ** CMAKE_INSTALL_PREFIX ** - /usr/local/itk

## CMakeLists.txt
```
cmake_minimum_required(VERSION 2.8.4)
project(TEST)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -std=c++11")

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(test1 MACOSX_BUNDLE main.cxx)

target_link_libraries(test1 ${VTK_LIBRARIES} ${ITK_LIBRARIES})
# If the simple include of VTK_LIBRARIES and ITK_LIBRARIES gives "Undefined symbols for architecture x86_64" error. Then include some other VTK libraries explicitly as shown below.
# target_link_libraries(test1 ${VTK_LIBRARIES} ${ITK_LIBRARIES} vtkRenderingVolume vtkRenderingOpenGL vtkRenderingVolumeOpenGL vtkInteractionStyle vtkRenderingFreeType vtkRenderingFreeTypeOpenGL)

```
