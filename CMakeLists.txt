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

# Generate icons before building
find_program(PYTHON_EXECUTABLE python3 python)
if(PYTHON_EXECUTABLE)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.png
               ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_icon.py
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_icon.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating icons"
    )
    add_custom_target(generate_icons 
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.png
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico)
else()
    message(WARNING "Python not found. Icons will not be generated automatically.")
    add_custom_target(generate_icons)
endif()

qt6_add_executable(MouseCross ${SOURCES} ${HEADERS} ${RESOURCES})
add_dependencies(MouseCross generate_icons)

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