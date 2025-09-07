cmake_minimum_required(VERSION 3.16)
project(MouseCross VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

set(SOURCES
    src/main.cpp
    src/MouseCrossApp.cpp
    src/CrosshairOverlay.cpp
    src/CrosshairRenderer.cpp
    src/WelcomeDialog.cpp
    src/SettingsDialog.cpp
    src/AboutDialog.cpp
    src/SettingsManager.cpp
    src/AutoStart.cpp
)

set(HEADERS
    src/MouseCrossApp.h
    src/CrosshairOverlay.h
    src/CrosshairRenderer.h
    src/WelcomeDialog.h
    src/SettingsDialog.h
    src/AboutDialog.h
    src/SettingsManager.h
    src/AutoStart.h
)

# Platform-specific sources
if(APPLE)
    list(APPEND SOURCES 
        src/MacOSCrosshairRenderer.mm
    )
    list(APPEND HEADERS 
        src/MacOSCrosshairRenderer.h
    )
elseif(WIN32)
    list(APPEND SOURCES 
        src/WindowsCrosshairRenderer.cpp
    )
    list(APPEND HEADERS 
        src/WindowsCrosshairRenderer.h
    )
else()
    # Linux/X11 uses the Windows renderer as fallback
    list(APPEND SOURCES 
        src/WindowsCrosshairRenderer.cpp
    )
    list(APPEND HEADERS 
        src/WindowsCrosshairRenderer.h
    )
endif()

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

target_link_libraries(MouseCross PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)

if(WIN32)
    set_target_properties(MouseCross PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Add windeployqt post-build step
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET MouseCross POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --release --no-opengl-sw --no-system-d3d-compiler --no-translations $<TARGET_FILE:MouseCross>
            COMMENT "Deploying Qt libraries with windeployqt")
    else()
        message(WARNING "windeployqt not found. Qt libraries will not be deployed automatically.")
    endif()
    
elseif(APPLE)
    set_target_properties(MouseCross PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
    
    # Add macdeployqt post-build step
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(MACDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET MouseCross POST_BUILD
            COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:MouseCross>
            COMMENT "Deploying Qt libraries with macdeployqt")
    else()
        message(WARNING "macdeployqt not found. Qt libraries will not be deployed automatically.")
    endif()
endif()

install(TARGETS MouseCross
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# CPack Configuration for deployment
set(CPACK_PACKAGE_NAME "MouseCross")
set(CPACK_PACKAGE_VENDOR "MouseCross")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Visual Mouse Locator for Accessibility")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://slohmaier.de/MouseCross")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Platform-specific packaging
if(WIN32)
    # Windows packaging options
    set(CPACK_GENERATOR "ZIP;WIX;NSIS")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "MouseCross")
    set(CPACK_PACKAGE_EXECUTABLES "MouseCross" "MouseCross")
    
    # WiX Configuration for MSI with proper update handling
    set(CPACK_WIX_UPGRADE_GUID "B8F5E4C2-7A9D-4E87-9F3B-2C8A1D5E6B7A")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico")
    set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_SOURCE_DIR}/deployment/windows/license.rtf")
    set(CPACK_WIX_PROGRAM_MENU_FOLDER "MouseCross")
    set(CPACK_WIX_PROPERTY_ARPHELPLINK "https://slohmaier.de/MouseCross")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://slohmaier.de/MouseCross")
    set(CPACK_WIX_PROPERTY_ARPCONTACT "Sebastian Lohmaier")
    # Enable major upgrade (removes old version before installing new)
    set(CPACK_WIX_PATCH "<?xml version='1.0' encoding='utf-8'?>
<CPackWiXPatch>
  <CPackWiXFragment Id='CM_CP_MOUSECROSS.EXE'>
    <RemoveFile Id='CM_RF_MOUSECROSS_SETTINGS' Name='*' On='uninstall' />
  </CPackWiXFragment>
</CPackWiXPatch>")
    
    # NSIS Configuration
    set(CPACK_NSIS_DISPLAY_NAME "MouseCross")
    set(CPACK_NSIS_PACKAGE_NAME "MouseCross")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\MouseCross.lnk' '$INSTDIR\\\\bin\\\\MouseCross.exe'"
    )
    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\MouseCross.lnk'"
    )
    
elseif(APPLE)
    # macOS packaging options
    set(CPACK_GENERATOR "DragNDrop;Bundle;productbuild")
    set(CPACK_BUNDLE_NAME "MouseCross")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")
    set(CPACK_DMG_VOLUME_NAME "MouseCross")
    set(CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos/DS_Store")
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos/dmg_background.png")
endif()

include(CPack)