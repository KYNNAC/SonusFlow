cmake_minimum_required(VERSION 3.16)

project(SonusFlow VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

# --- Create a Library for MiniAudio ---
# This is the ONLY place miniaudio.c should be compiled.
add_library(miniaudio_lib third_party/miniaudio.c)

# Tell the library where its own headers are. PUBLIC propagates this to linkers.
target_include_directories(miniaudio_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# Add the special decoder instructions directly to the library. PUBLIC propagates these.
target_compile_definitions(miniaudio_lib PUBLIC
    MA_ENABLE_MP3
    MA_ENABLE_FLAC
)
# --- End of MiniAudio Library Definition ---


set(TS_FILES SonusFlow_en_CA.ts)

# The list of sources for the main application.
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
    resources.qrc
)

# Create the executable from sources
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SonusFlow
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    add_executable(SonusFlow
        ${PROJECT_SOURCES}
    )
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


# Link SonusFlow against the Qt Widgets library, miniaudio_lib and Threads
target_link_libraries(SonusFlow PRIVATE Qt${QT_VERSION_MAJOR}::Widgets miniaudio_lib Threads::Threads)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SonusFlow)
endif()
set_target_properties(SonusFlow PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SonusFlow
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SonusFlow)
endif()
