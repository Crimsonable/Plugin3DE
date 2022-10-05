cmake_minimum_required(VERSION 3.20)
project(plugin_test)

#include(CTest)
#enable_testing()

add_definitions(-D_WINDOWS_SOURCE)
add_definitions(-D_LANGUAGE_CPLUSPLUS)
set(3DE_FRAMEWORK_ENV "D:/3DE")
set(3DE_LIB_DIR "D:/3DE/win_b64/code/lib")

set(EXPBLAS_PATH "D:/codes/expblas/Expblas")

add_definitions(-D_AFXDLL)
set(MFC_PATH "D:/vs2015/VC/atlmfc")
set(CMAKE_MFC_FLAG 2)



include_directories(${EXPBLAS_PATH} ${MFC_PATH}/include)
link_directories(${MFC_PATH}/lib)

add_subdirectory(utility)

# set(
#     INCLUDE_FRAMEWORK
#         "Public:SpecialAPI:"
#         "PublicGen:SpecialAPI:"
#         "Public:System:JS0GROUP"
# )

# set(INCLUDE_DIR_LIST "")
# set(LINK_LIB_LIST "")

# link_directories(${3DE_LIB_DIR})
# foreach(item ${INCLUDE_FRAMEWORK})
#     string(FIND ${item} ":" start)
#     string(SUBSTRING ${item} 0 ${start} OwnerShip)
#     math(EXPR start "${start}+1")

#     string(FIND ${item} ":" tail REVERSE)
#     math(EXPR len "${tail}-${start}")
#     string(SUBSTRING ${item} ${start} ${len} Framework)
#     math(EXPR tail "${tail}+1")

#     string(SUBSTRING ${item} ${tail} -1 Module)

#     if(OwnerShip STREQUAL "Public")
#         list(APPEND INCLUDE_DIR_LIST ${3DE_FRAMEWORK_ENV}/${Framework}/PublicInterfaces)
#     ELSEIF(OwnerShip STREQUAL "PublicGen")
#         list(APPEND INCLUDE_DIR_LIST ${3DE_FRAMEWORK_ENV}/${Framework}/PublicGenerated/win_b64)
#     ENDIF()

#     if(Module)
#         add_link_options(/DELAYLOAD:${Module}.dll)
#         list(APPEND LINK_LIB_LIST ${Module}.lib)
#     ENDIF()
# endforeach()

# include_directories(${INCLUDE_DIR_LIST})
# add_executable(plugin_test main.cpp)
# target_link_libraries(
#     plugin_test
#         delayimp.lib
#         ${LINK_LIB_LIST}
# )

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
