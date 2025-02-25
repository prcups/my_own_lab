cmake_minimum_required(VERSION 3.16)

project(HelloOpenGL VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
qt_standard_project_setup()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.ui
)

add_library(MyOpenGLClass
    myopenglclass.cpp
)

qt_add_executable(HelloOpenGL
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(HelloOpenGL PRIVATE Qt6::Widgets Qt6::OpenGLWidgets MyOpenGLClass)
target_link_libraries(MyOpenGLClass PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)

qt_finalize_executable(HelloOpenGL)
