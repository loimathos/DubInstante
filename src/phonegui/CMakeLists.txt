cmake_minimum_required(VERSION 3.16)

project(DubInstanteAndroid VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Qml Multimedia Widgets)
find_package(Qt6 REQUIRED COMPONENTS QuickControls2)

qt_add_executable(DubInstanteAndroid
    main.cpp
)

qt_add_qml_module(DubInstanteAndroid
    URI DubInstante
    VERSION 1.0
    QML_FILES Main.qml
)

# For Android APK generation properties
if(ANDROID)
    set_property(TARGET DubInstanteAndroid APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
                 "${CMAKE_CURRENT_SOURCE_DIR}/android")
endif()

target_link_libraries(DubInstanteAndroid PRIVATE
    Qt6::Quick
    Qt6::Qml
    Qt6::Multimedia
    Qt6::Widgets
    Qt6::QuickControls2
)
