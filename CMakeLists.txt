cmake_minimum_required(VERSION 3.16)
project(MouseCross VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    src/main.cpp
    src/MouseCrossApp.cpp
    src/CrosshairOverlay.cpp
    src/WelcomeDialog.cpp
    src/SettingsDialog.cpp
    src/AboutDialog.cpp
    src/SettingsManager.cpp
    src/AutoStart.cpp
)

set(HEADERS
    src/MouseCrossApp.h
    src/CrosshairOverlay.h
    src/WelcomeDialog.h
    src/SettingsDialog.h
    src/AboutDialog.h
    src/SettingsManager.h
    src/AutoStart.h
)

set(RESOURCES
    resources/resources.qrc
)

qt6_add_executable(MouseCross ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(MouseCross PRIVATE Qt6::Core Qt6::Widgets)

if(WIN32)
    set_target_properties(MouseCross PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(MouseCross PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
endif()

install(TARGETS MouseCross
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)