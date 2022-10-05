cmake_minimum_required(VERSION 3.20)

#set(3DE_FRAMEWORK_ENV "D:/3DE")
#set(3DE_LIB_DIR "D:/3DE/win_b64/code/lib")
message(${3DE_FRAMEWORK_ENV})
message(${3DE_LIB_DIR})

set(LIB_NAME "utility")

set(
    INCLUDE_FRAMEWORK
        "Public:SpecialAPI:"
        "PublicGen:SpecialAPI:"
        "Public:SystemTS:"
        "Public:System:JS0GROUP"
        "Public:System:JS0FM"
        "Public:Dialog:CATDlgInclude"
        "Public:DialogEngine:CATDialogEngine"
        "Public:VisualizationInterfaces:CATVisItf"
        "Public:CATMecModUseItf:CATMecModUseItf"
        "Public:AfrFoundation:CATAfrFoundation"
        "Public:CATMecModLiveUseItf:CATMecModLiveUseItf"
        "Public:CATPLMIdentificationAccess:CATPLMIdentificationAccess"
        "Public:CATPLMComponentInterfaces:CATPLMComponentInterfaces"
        "Public:KnowledgeInterfaces:KnowledgeItf"
        "Public:GeometricObjects:CATGMGeometricInterfaces"
        "Public:Mathematics:CATMathematics"
        "Public:ObjectModelerSystem:ObjectModelerSystem"
        "Public:CATPartUseItf:CATPartUseItf"
        "Public:DataCommonProtocolUse:DataCommonProtocolUse"
        "Public:CATGSMUseItf:CATGSMUseItf"
        "Public:GMOperatorsInterfaces:CATGMOperatorsInterfaces"
        "Public:CATMecModExtendItf:CATMecModExtendItf"
        "Public:ObjectModelerNavigator:CATObjectModelerNavigator"
        "Public:CATSketcherUseItf:CATSketcherUseItf"
        "Public:BSFBuildtimeData:CATIAV5Level"
)

set(INCLUDE_DIR_LIST "")
set(LINK_LIB_LIST "")

link_directories(${3DE_LIB_DIR})
foreach(item ${INCLUDE_FRAMEWORK})
    string(FIND ${item} ":" start)
    string(SUBSTRING ${item} 0 ${start} OwnerShip)
    math(EXPR start "${start}+1")

    string(FIND ${item} ":" tail REVERSE)
    math(EXPR len "${tail}-${start}")
    string(SUBSTRING ${item} ${start} ${len} Framework)
    math(EXPR tail "${tail}+1")

    string(SUBSTRING ${item} ${tail} -1 Module)

    if(OwnerShip STREQUAL "Public")
        list(APPEND INCLUDE_DIR_LIST ${3DE_FRAMEWORK_ENV}/${Framework}/PublicInterfaces)
    ELSEIF(OwnerShip STREQUAL "PublicGen")
        list(APPEND INCLUDE_DIR_LIST ${3DE_FRAMEWORK_ENV}/${Framework}/PublicGenerated/win_b64)
    ENDIF()

    if(Module)
        add_link_options(/DELAYLOAD:${Module}.dll)
        list(APPEND LINK_LIB_LIST ${Module}.lib)
    ENDIF()
endforeach()

include_directories(${INCLUDE_DIR_LIST})
add_library(utility SHARED "utility_plugin.cpp" "utility_plugin.h")
target_compile_options(utility PUBLIC "/Zc:__cplusplus")
target_link_libraries(
    utility
        ${LINK_LIB_LIST}
)

#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
