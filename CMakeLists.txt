cmake_minimum_required(VERSION 3.16)
project(MouseCross
    VERSION 0.1.0
    DESCRIPTION "A crosshair overlay application for visually impaired users"
    HOMEPAGE_URL "https://slohmaier.de/mousecross"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Option for App Store static linking
option(APPSTORE_BUILD "Build for Mac App Store with static Qt linking" OFF)

# Configure Qt linking strategy
if(APPSTORE_BUILD AND APPLE)
    message(STATUS "Building for Mac App Store with bundled Qt (pseudo-static)")
    # For App Store compliance, we'll create a fully bundled app
    # This is equivalent to static linking from a distribution perspective
    add_definitions(-DAPPSTORE_BUILD)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network LinguistTools)

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

# Add static plugin initialization for App Store builds
if(APPSTORE_BUILD AND APPLE)
    list(APPEND SOURCES src/static_plugins.cpp)
endif()

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
               ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon_hires.png
               ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico
               ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/create_icon.py
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/create_icon.py
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.svg
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating icons from programmatic design"
    )
    add_custom_target(generate_icons 
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.png
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon_hires.png
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico
                ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns)
else()
    message(WARNING "Python not found. Icons will not be generated automatically.")
    add_custom_target(generate_icons)
endif()

qt6_add_executable(MouseCross ${SOURCES} ${HEADERS} ${RESOURCES})
add_dependencies(MouseCross generate_icons)

target_link_libraries(MouseCross PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)

# Platform-specific linking
if(APPLE)
    target_link_libraries(MouseCross PRIVATE "-framework Carbon")

    # App Store configuration - enhanced frameworks
    if(APPSTORE_BUILD)
        target_link_libraries(MouseCross PRIVATE
            "-framework Cocoa"
            "-framework CoreFoundation"
            "-framework CoreServices"
            "-framework Security"
            "-framework SystemConfiguration"
        )
    endif()
endif()

# Translation support
set(TS_FILES
    translations/mousecross_de.ts
)

qt6_add_translations(MouseCross
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
)

if(WIN32)
    set_target_properties(MouseCross PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Add windeployqt post-build step (architecture-aware)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    if(WINDEPLOYQT_EXECUTABLE)
        # Determine architecture for windeployqt
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
            set(WINDEPLOYQT_ARCH "--force --compiler-runtime")
        else()
            set(WINDEPLOYQT_ARCH "")
        endif()

        add_custom_command(TARGET MouseCross POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --release --no-opengl-sw --no-system-d3d-compiler --no-translations ${WINDEPLOYQT_ARCH} $<TARGET_FILE:MouseCross>
            COMMENT "Deploying Qt libraries with windeployqt for ${CMAKE_GENERATOR_PLATFORM}")
    else()
        message(WARNING "windeployqt not found. Qt libraries will not be deployed automatically.")
    endif()
    
elseif(APPLE)
    # Basic macOS bundle properties
    set_target_properties(MouseCross PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_ICON_FILE app_icon.icns
    )

    # App Store specific bundle properties
    if(APPSTORE_BUILD)
        set_target_properties(MouseCross PROPERTIES
            LINK_FLAGS "-dead_strip -Wl,-rpath,@executable_path/../Frameworks"
        )
    endif()

    # Copy icon to bundle Resources directory
    add_custom_command(TARGET MouseCross POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns
            $<TARGET_BUNDLE_DIR:MouseCross>/Contents/Resources/app_icon.icns
        COMMENT "Copying app icon to bundle")

    # Add macdeployqt post-build step (skip for App Store builds - handled by build script)
    if(NOT APPSTORE_BUILD)
        find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS ${Qt6_DIR}/../../../bin)
        if(MACDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET MouseCross POST_BUILD
                COMMAND ${MACDEPLOYQT_EXECUTABLE} $<TARGET_BUNDLE_DIR:MouseCross>
                COMMENT "Deploying Qt libraries with macdeployqt")
        else()
            message(WARNING "macdeployqt not found. Qt libraries will not be deployed automatically.")
        endif()
    endif()
endif()

install(TARGETS MouseCross
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# Install Qt dependencies for packaging
if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    # Run windeployqt during install step to capture dependencies for CPack (architecture-aware)
    install(CODE "
        # Determine architecture for windeployqt
        if(\"${CMAKE_GENERATOR_PLATFORM}\" STREQUAL \"ARM64\")
            set(WINDEPLOYQT_ARCH \"--force --compiler-runtime\")
        else()
            set(WINDEPLOYQT_ARCH \"\")
        endif()

        execute_process(
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --release --no-opengl-sw --no-system-d3d-compiler --no-translations \${WINDEPLOYQT_ARCH} \${CMAKE_INSTALL_PREFIX}/bin/MouseCross.exe
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            message(FATAL_ERROR \"windeployqt failed: \${result}\")
        endif()
    ")
    
    # Install Qt DLLs and plugins that windeployqt copies
    install(DIRECTORY $<TARGET_FILE_DIR:MouseCross>/
        DESTINATION bin
        FILES_MATCHING 
        PATTERN "*.dll"
        PATTERN "platforms/"
        PATTERN "styles/"
        PATTERN "imageformats/"
        PATTERN "iconengines/"
        PATTERN "generic/"
        PATTERN "networkinformation/"
        PATTERN "tls/"
        PATTERN "*.exe" EXCLUDE
        PATTERN "MouseCross.exe" EXCLUDE
    )
endif()

# CPack Configuration for deployment
set(CPACK_PACKAGE_NAME "MouseCross")
set(CPACK_PACKAGE_VENDOR "Stefan Lohmaier")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Visual Mouse Locator for Accessibility")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://slohmaier.de/mousecross")
set(CPACK_PACKAGE_CONTACT "Stefan Lohmaier <stefan@slohmaier.de>")
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
    set(CPACK_WIX_PROPERTY_ARPCONTACT "Stefan Lohmaier")
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
    set(CPACK_GENERATOR "DragNDrop;Bundle")
    set(CPACK_BUNDLE_NAME "MouseCross")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.icns")
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")
    set(CPACK_DMG_VOLUME_NAME "MouseCross")
    # Disable problematic DS_Store and background for now
    # set(CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos/DS_Store")
    # set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/deployment/macos/dmg_background.png")
endif()

include(CPack)