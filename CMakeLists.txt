project(wiregraph)
cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND QT_MODULES Core Qml Quick QuickControls2 Svg)
find_package(Qt6 "6.2" COMPONENTS ${QT_MODULES} REQUIRED)
set(QT_PREFIXED_MODULES ${QT_MODULES})
list(TRANSFORM QT_PREFIXED_MODULES PREPEND "Qt6::")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# PipeWire
find_package(PkgConfig REQUIRED)
pkg_check_modules(PipeWire REQUIRED IMPORTED_TARGET "libpipewire-0.3")

include_directories("src" "src/Models")

file(GLOB_RECURSE SRCS "src/*.cpp")
file(GLOB_RECURSE HDRS "src/*.hpp")
file(GLOB_RECURSE QMLS RELATIVE "${CMAKE_SOURCE_DIR}" "src/qml/*.qml")

list(APPEND RSRC "icons.qrc")
qt6_add_resources(RESOURCES ${RSRC})

qt_add_executable(${PROJECT_NAME} ${SRCS} ${RESOURCES})

qt_add_qml_module(${PROJECT_NAME} URI "Backend" VERSION 1.0 QML_FILES ${QMLS} SOURCES "src/Models/device_model.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_PREFIXED_MODULES} PkgConfig::PipeWire)

# install
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

install(FILES "assets/${PROJECT_NAME}.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

install(FILES "assets/${PROJECT_NAME}.svg" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps")
list(APPEND ICON_SIZES 16 32 48 64 128 256 512)
foreach(ICON_SIZE IN LISTS ICON_SIZES)
	list(APPEND ICON_COMMANDS COMMAND "convert" "${CMAKE_SOURCE_DIR}/assets/${PROJECT_NAME}.svg" "-resize" "${ICON_SIZE}x${ICON_SIZE}" "${ICON_SIZE}.png")
	install(FILES "${CMAKE_BINARY_DIR}/${ICON_SIZE}.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps" RENAME "${PROJECT_NAME}.png")
endforeach()
add_custom_target(linux-desktop-integration ${ICON_COMMANDS})
