cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ODEPluginTest
        VERSION 0.0.1 
        LANGUAGES C CXX
)

configure_file(
    "include/version.h.in"
    "version.h"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-O3")
#set(CMAKE_CXX_FLAGS "-fsanitize=address,undefined -fno-omit-frame-pointer -g -O1")

file(GLOB_RECURSE SOURCES src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "src/nar.cpp")
list(FILTER SOURCES EXCLUDE REGEX "src/lorenz.cpp")

# # Add GSL - this CMakeLists.txt is modified from Ben Haller's SLiM software (https://github.com/MesserLab/SLiM), released under GPL 2.0
set(TARGET_NAME_GSL gsl)
file(GLOB_RECURSE GSL_SOURCES ${PROJECT_SOURCE_DIR}/gsl/*.c ${PROJECT_SOURCE_DIR}/gsl/*/*.c)
message("Path = ${PROJECT_SOURCE_DIR}/gsl")
set(GSL_INCLUDES ${PROJECT_SOURCE_DIR}/gsl ${PROJECT_SOURCE_DIR}/gsl/specfunc ${PROJECT_SOURCE_DIR}/gsl/blas ${PROJECT_SOURCE_DIR}/gsl/rng ${PROJECT_SOURCE_DIR}/gsl/cdf ${PROJECT_SOURCE_DIR}/gsl/vector ${PROJECT_SOURCE_DIR}/gsl/err ${PROJECT_SOURCE_DIR}/gsl/sys ${PROJECT_SOURCE_DIR}/gsl/randist ${PROJECT_SOURCE_DIR}/gsl/matrix ${PROJECT_SOURCE_DIR}/gsl/cblas ${PROJECT_SOURCE_DIR}/gsl/complex ${PROJECT_SOURCE_DIR}/gsl/block ${PROJECT_SOURCE_DIR}/gsl/interpolation ${PROJECT_SOURCE_DIR}/gsl/linalg ${PROJECT_SOURCE_DIR}/gsl/permutation ${PROJECT_SOURCE_DIR}/gsl/ode-initval2 /usr/local/include)
add_library(${TARGET_NAME_GSL} STATIC ${GSL_SOURCES})
target_include_directories(${TARGET_NAME_GSL} PUBLIC ${GSL_INCLUDES})
set_target_properties(gsl PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
${GSL_INCLUDES}
${PROJECT_BINARY_DIR}
"${PROJECT_SOURCE_DIR}/include"
)
if(CMAKE_DL_LIBS)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS} gsl)
else()
target_link_libraries(${PROJECT_NAME} PUBLIC gsl)
endif()

# Test libraries
add_library(NAR MODULE "${PROJECT_SOURCE_DIR}/src/nar.cpp")
add_library(Lorenz MODULE "${PROJECT_SOURCE_DIR}/src/lorenz.cpp")


target_include_directories(NAR PUBLIC 
#${GSL_INCLUDES}
"${PROJECT_BINARY_DIR}"
"${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(NAR PRIVATE gsl)

target_include_directories(Lorenz PUBLIC
#${GSL_INCLUDES}
"${PROJECT_BINARY_DIR}"
"${PROJECT_SOURCE_DIR}/include"
)

target_link_libraries(Lorenz PRIVATE gsl)


install(TARGETS ${PROJECT_NAME} DESTINATION bin)

include(CTest)
enable_testing()
message(STATUS "${PROJECT_SOURCE_DIR}")

# Determine the shared library extension
if(WIN32)
    set(SHARED_EXT "dll")
else()
    set(SHARED_EXT "so")
endif()

# Write multi file write for testing
file(WRITE "lib_multi_test.csv"
"./libNAR.${SHARED_EXT}\n"
"./libLorenz.${SHARED_EXT}\n"
)

# test NAR
add_test(
  NAME testNAR
  COMMAND ${PROJECT_NAME} -p ${PROJECT_SOURCE_DIR}/pars_nar_test.csv -n ${PROJECT_SOURCE_DIR}/vars_nar_test.csv -o ${PROJECT_SOURCE_DIR}/out_nar.csv -i ${PROJECT_BINARY_DIR}/libNAR.${SHARED_EXT}
)

# test Lorenz
add_test(
  NAME testLorenz
  COMMAND ${PROJECT_NAME} -p ${PROJECT_SOURCE_DIR}/pars_lor_test.csv -n ${PROJECT_SOURCE_DIR}/vars_lor_test.csv -o ${PROJECT_SOURCE_DIR}/out_lorenz.csv -i ${PROJECT_BINARY_DIR}/libLorenz.${SHARED_EXT}
)

# Test multi-library version
add_test(
  NAME testMulti
  COMMAND ${PROJECT_NAME} -p ${PROJECT_SOURCE_DIR}/pars_multi_test.csv -n ${PROJECT_SOURCE_DIR}/vars_multi_test.csv -o ${PROJECT_SOURCE_DIR}/out_multi.csv -i ${PROJECT_SOURCE_DIR}/lib_multi_test.csv
)

# Setup for github actions
set_tests_properties(testNAR testLorenz testMulti PROPERTIES LABELS "gh")


