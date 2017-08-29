cmake_minimum_required(VERSION 3.4.1)
project(rib_locator)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(MAYA_VERSION 2017)
set(CUSTOM_DEFINITIONS "REQUIRE_IOSTREAM;_BOOL")
set(MAYA_INSTALL_BASE_SUFFIX "")
set(MAYA_INC_SUFFIX "include")
set(MAYA_LIB_SUFFIX "lib")
set(MAYA_INSTALL_BASE_SUFFIX "")

if(WIN32)
    set(MAYA_INSTALL_BASE_DEFAULT "C:/Program Files/Autodesk")
elseif(APPLE)
    set(MAYA_INSTALL_BASE_DEFAULT /Applications/Autodesk)
else()
    set(MAYA_INSTALL_BASE_DEFAULT /usr/autodesk)
    if(MAYA_VERSION LESS 2016)
        SET(MAYA_INSTALL_BASE_SUFFIX -x64)
    endif()
endif()
set(MAYA_INSTALL_BASE_PATH ${MAYA_INSTALL_BASE_DEFAULT})
set(MAYA_LOCATION
   ${MAYA_INSTALL_BASE_PATH}/maya${MAYA_VERSION}${MAYA_INSTALL_BASE_SUFFIX})

find_package(OpenGL REQUIRED)

if(WIN32)
    add_definitions(-D_WIN32)
elseif(APPLE)
    add_definitions(-DOSMac_)
else()
    add_definitions(-DLINUX)
endif()

set(MAYA_LIBRARIES ${LIBRARIES}
    "Foundation"
    "OpenMaya"
    "OpenMayaAnim"
    "OpenMayaUI"
    "OpenMayaRender"
    ${OPENGL_LIBRARIES})

set(_MAYA_LOCATION
    ${MAYA_INSTALL_BASE_PATH}/maya${MAYA_VERSION}${MAYA_INSTALL_BASE_SUFFIX})

set(_PROJECT ${PROJECT_NAME})

include_directories(${_MAYA_LOCATION}/${MAYA_INC_SUFFIX}
                    ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS}
                    "${CMAKE_CURRENT_BINARY_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}/utils")

link_directories(${_MAYA_LOCATION}/${MAYA_LIB_SUFFIX})


BISON_TARGET(rib_parser
             parser/rib_parser.y
             ${CMAKE_CURRENT_BINARY_DIR}/rib_parser.tab.cc)
FLEX_TARGET(rib_lexer
            parser/rib_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/rib_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(rib_lexer rib_parser)

set(MAYA_SOURCE_FILES
    "maya/rib_locator.cc"
    "maya/rib_locator_main.cc"
    "utils/primitives.cc"
)

add_library(${_PROJECT} SHARED ${MAYA_SOURCE_FILES})

set_target_properties(${_PROJECT} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${_PROJECT} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

if(WIN32)
    set_target_properties(${_PROJECT} PROPERTIES SUFFIX ".mll" )
    set_target_properties(${_PROJECT} PROPERTIES LINK_FLAGS
        "/export:initializePlugin /export:uninitializePlugin" )
elseif(APPLE)
    set_target_properties(${_PROJECT} PROPERTIES PREFIX "" )
    set_target_properties(${_PROJECT} PROPERTIES SUFFIX ".bundle" )
else()
    set_target_properties(${_PROJECT} PROPERTIES PREFIX "" )
endif()


add_library(rib_driver
    STATIC
    parser/rib_driver.cc
    ${FLEX_rib_lexer_OUTPUTS}
    ${BISON_rib_parser_OUTPUTS}
)

target_include_directories(rib_driver
    PRIVATE . ${CMAKE_CURRENT_BINARY_DIR}/parser)

set_target_properties(rib_driver PROPERTIES COMPILE_FLAGS
                      "${CMAKE_CXX_FLAGS} -fPIC")

target_link_libraries(${_PROJECT} rib_driver ${MAYA_LIBRARIES})

add_executable(rib_parser main.cc)
target_include_directories(rib_parser PRIVATE . ${CMAKE_CURRENT_BINARY_DIR}/parser)
target_link_libraries(rib_parser rib_driver)

