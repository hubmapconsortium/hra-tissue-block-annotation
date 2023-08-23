cmake_minimum_required(VERSION 3.9)
project(MESH_PROCESSING)

set(CMAKE_BUILD_TYPE "Release")

find_package(CGAL REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

file(GLOB GEOMETRY_SRC_FILES geometry/*.cpp)
add_library(geometry ${GEOMETRY_SRC_FILES})
target_include_directories(geometry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/geometry)
target_link_libraries(geometry PUBLIC Eigen3::Eigen)
target_link_libraries(geometry PUBLIC CGAL::CGAL)

add_executable(mesh_checker mesh_checker.cpp)
add_executable(mesh_hole_filling mesh_hole_filling.cpp)

target_link_libraries(mesh_checker geometry)
target_link_libraries(mesh_checker CGAL::CGAL)

target_link_libraries(mesh_hole_filling geometry)
target_link_libraries(mesh_hole_filling CGAL::CGAL)
target_link_libraries(mesh_hole_filling Eigen3::Eigen)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(mesh_checker ${Boost_LIBRARIES})
    target_link_libraries(mesh_hole_filling ${Boost_LIBRARIES})
endif()